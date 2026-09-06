#include "stage.h"
#include "enemy.h"
#include "powerup.h"

// SPEC.md §18/§40.4 — Space Colony. Length is exact (48,000px / 2px/frame =
// 24000 frames = 400s); GAME_SPEC.md's own stated "~5:00" duration doesn't
// match its own length+scroll numbers either (same class of internal
// inconsistency already flagged for Stage 1/2 — length/scroll is treated
// as authoritative, duration as an approximate/rounded figure). §40.4 also
// gives Stage 3 a "2-3 px/frame" scroll range; StageDef only has a single
// scrollSpeed field with no mid-stage ramp (NS-M07-005's already-documented
// "maximum" gap), so this uses the flat, honest single value of 2 rather
// than inventing a ramp mechanism. No 0-25/25-50/50-75/75-100% distribution
// table exists for Stage 3 the way §40.2 gave one for Stage 1 — this
// table's banding is a reasonable extrapolation of the same shape, not a
// literal spec requirement. See PROGRESS.md for the actual counts this
// table produces vs. the spec's targets (130-170 kills, 6 enemy types, 14
// power-ups, etc.).
static const SpawnEvent stage3Events[] = {
    // --- 0-25% (0-6000f): intro to the colony corridors. Fighter/Turret,
    // one early power-up. Max ~5 simultaneous.
    {  240, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_FIGHTER },
    {  360, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_FIGHTER },
    { 1080, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_P     },
    { 1680, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_TURRET  },
    { 1920, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_TURRET  },
    { 2700, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_FIGHTER },
    { 2700, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_FIGHTER },
    { 2700, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_FIGHTER },
    { 3600, SPAWN_TYPE_POWERUP, 300,  80, POWERUP_WIDE  },
    { 4500, SPAWN_TYPE_ENEMY,   300,  50, ENEMY_TURRET  },
    { 4500, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_FIGHTER },
    { 4500, SPAWN_TYPE_ENEMY,   300, 150, ENEMY_TURRET  },
    { 5700, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_1UP   }, // SPEC.md §40.4: 0-1 per stage

    // --- 25-50% (6000-12000f): + Bomber, first Shield encounters.
    // Checkpoint at 9600f (40%). Max 8 enemies/12 projectiles (design
    // target, see PROGRESS.md's measured-vs-designed caveat from M08).
    { 6600, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_BOMBER  },
    { 7200, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    { 7200, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_FIGHTER },
    { 7800, SPAWN_TYPE_POWERUP, 300, 120, POWERUP_BOMB  },
    { 8400, SPAWN_TYPE_ENEMY,   300, 120, ENEMY_SHIELD  },
    { 9000, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_TURRET  },
    { 9000, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_TURRET  },
    {10200, SPAWN_TYPE_ENEMY,   300,  80, ENEMY_BOMBER  },
    {10800, SPAWN_TYPE_POWERUP, 300,  90, POWERUP_LASER },
    {11400, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_SHIELD  },

    // --- 50-75% (12000-18000f): Swarm groups, Charger combos. Checkpoint
    // at 18000f (75%). Max 10 enemies/18 projectiles (design target).
    {12000, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_SWARM   },
    {12000, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_SWARM   },
    {12000, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },
    {12900, SPAWN_TYPE_ENEMY,   300, 130, ENEMY_CHARGER },
    {13800, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_SPEED },
    {14700, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_BOMBER  },
    {14700, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_TURRET  },
    {15600, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_SWARM   },
    {15600, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_SWARM   },
    {15600, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },
    {16500, SPAWN_TYPE_POWERUP, 300,  90, POWERUP_HOMING},
    {17400, SPAWN_TYPE_ENEMY,   300,  50, ENEMY_SHIELD  },
    {17400, SPAWN_TYPE_ENEMY,   300, 150, ENEMY_CHARGER },

    // --- 75-100% (18000-24000f): boss preparation. Max 12 enemies/20
    // projectiles (design target), mini-boss near the end of this band.
    {18600, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_FIGHTER },
    {18600, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_TURRET  },
    {18600, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_BOMBER  },
    {19500, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_FLAME },
    {20400, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_SWARM   },
    {20400, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },
    {20400, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_CHARGER },
    {21600, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_MINIBOSS}, // SPEC.md §40.4: 1 mini-boss
    {22800, SPAWN_TYPE_POWERUP, 300,  90, POWERUP_P     },
    {23400, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_SHIELD  },
    {23400, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_FIGHTER },
};

const StageDef stage3 = {
    24000,                                            // lengthFrames: 48,000px / 2px/frame (400s)
    2,                                                 // scrollSpeed: SPEC.md §40.4 gives 2-3, engine only has a flat value (see above)
    stage3Events,
    sizeof(stage3Events) / sizeof(stage3Events[0]),
    { 9600, 18000 },                                   // checkpointFrames: ~40%, ~75% of 24000
    3,                                                 // bossId: Industrial Core (boss_data.c)
    3,                                                 // backgroundId: Space Colony (pipe tile)
    3                                                  // musicId: Space Colony theme (audio.c)
};
