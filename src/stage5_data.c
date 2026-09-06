#include "stage.h"
#include "enemy.h"
#include "powerup.h"

// SPEC.md §20/§40.6 — Helios Fleet. Length is exact (57,600px / 3px/frame =
// 19200 frames = 320s), the closest any stage's stated duration ("~5:30"
// = 330s) has come to matching its own length+scroll numbers so far — see
// PROGRESS.md for the running list of these mismatches across all 5
// stages. No 0-25/25-50/50-75/75-100% distribution table exists for
// Stage 5 the way §40.2 gave one for Stage 1 — this table's banding is a
// reasonable extrapolation, not a literal spec requirement.
//
// "8 tipos de enemigo" (§40.6): the engine only has 7 combat EnemyType
// values (Drone/Fighter/Bomber/Turret/Swarm/Charger/Shield) plus
// ENEMY_MINIBOSS as its own enum value — using all 8, with 2 separate
// mini-boss spawns per §40.6's "Mini-bosses: 2", honestly reaches "8
// types" without inventing "Cruceros"/"Misiles" as fake new entities (see
// PROGRESS.md's M11 summary).
//
// "Formaciones" (diagonal/V/line/circle/cross/wave, §40.6) are approximated
// with same-frame multi-enemy SpawnEvents at staggered y offsets — the
// same technique M05/M07 already used for "group movement"/"formations",
// not a new formation-shape system.
static const SpawnEvent stage5Events[] = {
    // --- 0-25% (0-4800f): high-density intro. Fighter/Turret in line/V
    // formations. Max ~5-8 simultaneous (§40.6 has no lower band, so this
    // stays denser than Stage 1-4's own intros from the start).
    {  240, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_FIGHTER },
    {  240, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_FIGHTER },
    {  240, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_FIGHTER }, // line formation
    {  960, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_P     },
    { 1440, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_TURRET  },
    { 1440, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_TURRET  },
    { 2160, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_FIGHTER },
    { 2160, SPAWN_TYPE_ENEMY,   300, 110, ENEMY_FIGHTER },
    { 2160, SPAWN_TYPE_ENEMY,   300, 130, ENEMY_FIGHTER }, // V formation (converging y)
    { 3000, SPAWN_TYPE_POWERUP, 300,  80, POWERUP_WIDE  },
    { 3600, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_BOMBER  },
    { 3600, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_BOMBER  }, // "cruceros": approximated with Bomber, SPEC.md gives no dedicated cruiser entity
    { 4560, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_1UP   }, // SPEC.md §40.6: 1 per stage

    // --- 25-50% (4800-9600f): + Swarm diagonal/circle formations.
    // Checkpoint at 7680f (40%).
    { 5280, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_SWARM   },
    { 5280, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_SWARM   },
    { 5280, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },
    { 5280, SPAWN_TYPE_ENEMY,   300, 130, ENEMY_SWARM   }, // circle-ish cluster
    { 6000, SPAWN_TYPE_POWERUP, 300, 120, POWERUP_BOMB  },
    { 6480, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_FIGHTER },
    { 6600, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_FIGHTER },
    { 6720, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_FIGHTER }, // diagonal formation (staggered frame+y)
    { 7200, SPAWN_TYPE_ENEMY,   300, 150, ENEMY_TURRET  },
    { 7560, SPAWN_TYPE_ENEMY,   300,  50, ENEMY_DRONE   },
    { 7560, SPAWN_TYPE_ENEMY,   300, 150, ENEMY_DRONE   },
    { 7920, SPAWN_TYPE_POWERUP, 300,  90, POWERUP_LASER },
    { 8400, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_CHARGER },
    { 8400, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_CHARGER },

    // --- 50-75% (9600-14400f): Shield + Bomber combos, cross-attack
    // formation. Checkpoint at 14400f (75%).
    { 9600, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_SHIELD  },
    { 9600, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_SHIELD  },
    {10320, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_SPEED },
    {10800, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_BOMBER  },
    {11280, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_FIGHTER },
    {11280, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_FIGHTER },
    {11280, SPAWN_TYPE_ENEMY,   300, 130, ENEMY_FIGHTER },
    {11280, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_FIGHTER }, // cross-attack formation (4 lanes)
    {12000, SPAWN_TYPE_POWERUP, 300,  90, POWERUP_HOMING},
    {12480, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_SWARM   },
    {12480, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },
    {12480, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_SWARM   },
    {13200, SPAWN_TYPE_ENEMY,   300,  50, ENEMY_TURRET  },
    {13200, SPAWN_TYPE_ENEMY,   300, 150, ENEMY_TURRET  },
    {13920, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_MINIBOSS}, // SPEC.md §40.6: 1st of 2 mini-bosses

    // --- 75-100% (14400-19200f): boss preparation, peak density (wave
    // formation), 2nd mini-boss near the end.
    {14880, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_FIGHTER },
    {15000, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_FIGHTER },
    {15120, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_FIGHTER },
    {15240, SPAWN_TYPE_ENEMY,   300, 130, ENEMY_FIGHTER }, // wave formation (staggered frame, ascending y)
    {15600, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_FLAME },
    {16320, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_BOMBER  },
    {16320, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_SHIELD  },
    {17040, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_MINIBOSS}, // SPEC.md §40.6: 2nd of 2 mini-bosses
    {17760, SPAWN_TYPE_POWERUP, 300,  90, POWERUP_P     },
    {18480, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_CHARGER },
    {18480, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_TURRET  },
};

const StageDef stage5 = {
    19200,                                             // lengthFrames: 57,600px / 3px/frame (320s)
    3,                                                  // scrollSpeed: SPEC.md §40.6 normal
    stage5Events,
    sizeof(stage5Events) / sizeof(stage5Events[0]),
    { 7680, 14400 },                                    // checkpointFrames: ~40%, ~75% of 19200
    5,                                                  // bossId: Admiral X (boss_data.c)
    5,                                                  // backgroundId: plain starfield ("gran batalla espacial")
    5                                                   // musicId: Helios Fleet theme (audio.c)
};
