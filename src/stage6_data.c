#include "stage.h"
#include "enemy.h"
#include "powerup.h"
#include "asteroid.h"

// SPEC.md §21/§40.7 — Helios Core, the campaign's climax. Length is exact
// (48,000px / 3px/frame, the mid-point of SPEC.md's own "2-4 px/frame"
// range for this stage — StageDef has one flat scrollSpeed field, no
// mid-stage ramp, NS-M07-005 — = 16000 frames = 267s); GAME_SPEC.md's own
// stated "~5:00" duration (300s) doesn't match its own length+scroll
// numbers either, same class of internal inconsistency flagged for every
// earlier stage. No 0-25/25-50/50-75/75-100% distribution table exists
// for Stage 6 the way §40.2 gave one for Stage 1 — this table's banding
// is a reasonable extrapolation, not a literal spec requirement.
//
// SPEC.md's explicit direction for this stage — "la dificultad debe
// provenir de patrones/posicionamiento/velocidad/combinaciones, no de
// enemigos con cantidades absurdas de HP" — is honored by leaning on the
// existing roster's fastest/densest options (Charger, tight Swarm/
// Fighter/Turret combinations) rather than inflating HP values.
//
// "Tipos de enemigo: 9" (§40.7): reuses all 7 combat EnemyType values +
// ENEMY_MINIBOSS (8, same reasoning as Stage 5 — see PROGRESS.md) + M11's
// Asteroid system reused here as SPEC.md's own "obstaculos" requirement,
// counted as the 9th distinct threat type. "Lasers" have no dedicated
// environmental-hazard entity (no scenery-hazard system exists — NS-9/
// NS-15); approximated by the existing Laser weapon being readily
// available via power-ups placed in this table, not a new stage hazard.
static const SpawnEvent stage6Events[] = {
    // --- 0-25% (0-4000f): already dense from the start (SPEC.md: this
    // stage is the campaign's climax, no gentle ramp-in expected).
    {  240, SPAWN_TYPE_ENEMY,    300,  40, ENEMY_FIGHTER   },
    {  240, SPAWN_TYPE_ENEMY,    300,  90, ENEMY_TURRET    },
    {  240, SPAWN_TYPE_ENEMY,    300, 140, ENEMY_FIGHTER   },
    {  720, SPAWN_TYPE_ASTEROID, 340, 100, ASTEROID_LARGE  }, // "obstaculos"
    {  960, SPAWN_TYPE_POWERUP,  300, 100, POWERUP_P       },
    { 1440, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_CHARGER   }, // "enemigos rapidos"
    { 1440, SPAWN_TYPE_ENEMY,    300, 160, ENEMY_CHARGER   },
    { 2160, SPAWN_TYPE_ENEMY,    300,  40, ENEMY_SWARM     },
    { 2160, SPAWN_TYPE_ENEMY,    300,  70, ENEMY_SWARM     },
    { 2160, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_SWARM     },
    { 2880, SPAWN_TYPE_POWERUP,  300,  80, POWERUP_LASER   }, // "lasers": the weapon, no environmental hazard entity exists
    { 3360, SPAWN_TYPE_ASTEROID, 340,  60, ASTEROID_SMALL  },
    { 3360, SPAWN_TYPE_ASTEROID, 340, 160, ASTEROID_SMALL  },
    { 3840, SPAWN_TYPE_ENEMY,    300,  90, ENEMY_BOMBER    },

    // --- 25-50% (4000-8000f): + Shield combos. Checkpoint at 6400f (40%).
    { 4320, SPAWN_TYPE_ENEMY,    300,  40, ENEMY_SHIELD    },
    { 4320, SPAWN_TYPE_ENEMY,    300, 160, ENEMY_SHIELD    },
    { 5040, SPAWN_TYPE_POWERUP,  300, 120, POWERUP_BOMB    },
    { 5520, SPAWN_TYPE_ENEMY,    300,  50, ENEMY_TURRET    },
    { 5520, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_FIGHTER   },
    { 5520, SPAWN_TYPE_ENEMY,    300, 150, ENEMY_TURRET    },
    { 6000, SPAWN_TYPE_ASTEROID, 340,  90, ASTEROID_LARGE  },
    { 6480, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_CHARGER   },
    { 6480, SPAWN_TYPE_ENEMY,    300, 160, ENEMY_CHARGER   },
    { 7200, SPAWN_TYPE_POWERUP,  300,  90, POWERUP_HOMING  },
    { 7680, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_MINIBOSS  }, // SPEC.md §40.7: 1st of 2 mini-bosses

    // --- 50-75% (8000-12000f): peak combinations. Checkpoint at 12000f
    // (75%).
    { 8400, SPAWN_TYPE_ENEMY,    300,  40, ENEMY_SWARM     },
    { 8400, SPAWN_TYPE_ENEMY,    300,  70, ENEMY_SWARM     },
    { 8400, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_SWARM     },
    { 8400, SPAWN_TYPE_ENEMY,    300, 130, ENEMY_SWARM     },
    { 9120, SPAWN_TYPE_ASTEROID, 340,  50, ASTEROID_MEDIUM },
    { 9120, SPAWN_TYPE_POWERUP,  300, 100, POWERUP_FLAME   },
    { 9840, SPAWN_TYPE_ENEMY,    300,  40, ENEMY_FIGHTER   },
    { 9840, SPAWN_TYPE_ENEMY,    300,  90, ENEMY_TURRET    },
    { 9840, SPAWN_TYPE_ENEMY,    300, 140, ENEMY_BOMBER    },
    {10560, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_CHARGER   },
    {10560, SPAWN_TYPE_ENEMY,    300, 160, ENEMY_CHARGER   },
    {11280, SPAWN_TYPE_POWERUP,  300,  90, POWERUP_WIDE    },
    {11760, SPAWN_TYPE_ENEMY,    300,  50, ENEMY_SHIELD    },
    {11760, SPAWN_TYPE_ENEMY,    300, 150, ENEMY_SHIELD    },

    // --- 75-100% (12000-16000f): final approach. Max density, 2nd
    // mini-boss, then the Helios entrance itself (SpawnManager completing
    // triggers STATE_BOSS -> heliosDef, see boss_data.c).
    {12480, SPAWN_TYPE_ASTEROID, 340,  80, ASTEROID_LARGE  },
    {12960, SPAWN_TYPE_ENEMY,    300,  40, ENEMY_FIGHTER   },
    {12960, SPAWN_TYPE_ENEMY,    300,  90, ENEMY_FIGHTER   },
    {12960, SPAWN_TYPE_ENEMY,    300, 140, ENEMY_FIGHTER   },
    {13680, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_CHARGER   },
    {13680, SPAWN_TYPE_ENEMY,    300, 160, ENEMY_CHARGER   },
    {14160, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_MINIBOSS  }, // SPEC.md §40.7: 2nd of 2 mini-bosses
    {14880, SPAWN_TYPE_POWERUP,  300,  90, POWERUP_P       },
    {15360, SPAWN_TYPE_ENEMY,    300,  50, ENEMY_TURRET    },
    {15360, SPAWN_TYPE_ENEMY,    300, 150, ENEMY_TURRET    },
};

const StageDef stage6 = {
    16000,                                             // lengthFrames: 48,000px / 3px/frame (267s)
    3,                                                  // scrollSpeed: mid-point of SPEC.md §40.7's 2-4 range
    stage6Events,
    sizeof(stage6Events) / sizeof(stage6Events[0]),
    { 6400, 12000 },                                    // checkpointFrames: ~40%, ~75% of 16000
    6,                                                  // bossId: Helios (boss_data.c) — "final boss entrance", see PROGRESS.md
    6,                                                  // backgroundId: plain starfield (falls through scroll.c unchanged — Helios's own black/red/white sprite carries this stage's visual identity)
    6                                                   // musicId: Helios Core theme (audio.c)
};
