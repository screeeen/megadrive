#include "stage.h"
#include "enemy.h"
#include "powerup.h"

// SPEC.md §17/§40.3 — Red Desert. Length is exact (43,200px / 2px/frame =
// 21600 frames = 360s); GAME_SPEC.md's own stated "~4:45" duration for
// this stage doesn't match its own length+scroll numbers either (same
// class of internal inconsistency already flagged for Stage 1 in
// stage1_data.c — length/scroll is treated as the authoritative pair,
// duration as an approximate/rounded figure). No 0-25/25-50/50-75/75-100%
// distribution table exists for Stage 2 in GAME_SPEC.md the way §40.2 gave
// one for Stage 1, so this table's banding is a reasonable, honest
// extrapolation of the same shape, not a literal spec requirement. See
// PROGRESS.md for the actual counts this table produces vs. the spec's
// targets (120-150 kills, 5 enemy types, 12 power-ups, etc.).
static const SpawnEvent stage2Events[] = {
    // --- 0-25% (0-5400f): intro to the desert. Drone/Fighter, one early
    // power-up. Max ~5 simultaneous.
    {  240, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_DRONE   },
    {  360, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_DRONE   },
    {  480, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_DRONE   },
    {  720, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_DRONE   },
    {  960, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_P     },
    { 1200, SPAWN_TYPE_ENEMY,   300, 170, ENEMY_DRONE   },
    { 1500, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    { 1680, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_FIGHTER },
    { 1950, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_DRONE   },
    { 2200, SPAWN_TYPE_ENEMY,   300, 130, ENEMY_FIGHTER },
    { 2400, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_DRONE   },
    { 2400, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_DRONE   },
    { 2400, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_DRONE   },
    { 2850, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_FIGHTER },
    { 3300, SPAWN_TYPE_POWERUP, 300,  80, POWERUP_WIDE  },
    { 3600, SPAWN_TYPE_ENEMY,   300,  50, ENEMY_DRONE   },
    { 3600, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_DRONE   },
    { 3900, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_FIGHTER },
    { 4200, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    { 4200, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_FIGHTER },
    { 4650, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_DRONE   },
    { 5100, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_1UP   }, // SPEC.md §40.3: 1 per stage

    // --- 25-50% (5400-10800f): + Turret, first bursts. Checkpoint at
    // 8640f (40%). Max 8 enemies/12 projectiles (design target, see
    // PROGRESS.md's measured-vs-designed caveat from M08).
    { 5400, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_FIGHTER },
    { 5700, SPAWN_TYPE_ENEMY,   300, 150, ENEMY_FIGHTER },
    { 6000, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_TURRET  },
    { 6300, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_DRONE   },
    { 6600, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    { 6600, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_FIGHTER },
    { 6900, SPAWN_TYPE_ENEMY,   300, 120, ENEMY_TURRET  },
    { 7200, SPAWN_TYPE_POWERUP, 300, 120, POWERUP_BOMB  },
    { 7500, SPAWN_TYPE_ENEMY,   300,  50, ENEMY_FIGHTER },
    { 7800, SPAWN_TYPE_ENEMY,   300, 120, ENEMY_TURRET  },
    { 8100, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_DRONE   },
    { 8400, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_DRONE   },
    { 8400, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_DRONE   },
    { 8700, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_TURRET  },
    { 9000, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_FIGHTER },
    { 9300, SPAWN_TYPE_ENEMY,   300, 130, ENEMY_TURRET  },
    { 9600, SPAWN_TYPE_ENEMY,   300,  80, ENEMY_FIGHTER },
    { 9600, SPAWN_TYPE_ENEMY,   300, 150, ENEMY_TURRET  },
    {10200, SPAWN_TYPE_POWERUP, 300,  90, POWERUP_LASER },
    {10500, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },

    // --- 50-75% (10800-16200f): Charger introduced (SPEC.md §40.3's new
    // mechanic for this stage), Swarm groups. Checkpoint at 16200f (75%).
    // Max 10 enemies/18 projectiles (design target).
    {10800, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_CHARGER },
    {11100, SPAWN_TYPE_ENEMY,   300,  50, ENEMY_TURRET  },
    {11400, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_SWARM   },
    {11400, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_SWARM   },
    {11400, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },
    {11700, SPAWN_TYPE_ENEMY,   300, 150, ENEMY_FIGHTER },
    {12000, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_CHARGER },
    {12300, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_SPEED },
    {12600, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_SWARM   },
    {12600, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },
    {12900, SPAWN_TYPE_ENEMY,   300, 130, ENEMY_TURRET  },
    {13200, SPAWN_TYPE_ENEMY,   300, 130, ENEMY_CHARGER },
    {13500, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_FIGHTER },
    {13800, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    {13800, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_TURRET  },
    {14100, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_SWARM   },
    {14100, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_SWARM   },
    {14700, SPAWN_TYPE_POWERUP, 300,  90, POWERUP_HOMING},
    {15000, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_CHARGER },
    {15300, SPAWN_TYPE_ENEMY,   300,  50, ENEMY_TURRET  },
    {15600, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_SWARM   },
    {15600, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_SWARM   },
    {15600, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },

    // --- 75-100% (16200-21600f): boss preparation. Max 12 enemies/20
    // projectiles (design target), mini-boss near the end of this band.
    {16200, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    {16500, SPAWN_TYPE_ENEMY,   300, 150, ENEMY_TURRET  },
    {16800, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_CHARGER },
    {16800, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_TURRET  },
    {17100, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_SWARM   },
    {17100, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },
    {17400, SPAWN_TYPE_ENEMY,   300, 130, ENEMY_FIGHTER },
    {17700, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_FLAME },
    {18000, SPAWN_TYPE_ENEMY,   300,  50, ENEMY_CHARGER },
    {18300, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    {18300, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },
    {18300, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_SWARM   },
    {18600, SPAWN_TYPE_ENEMY,   300, 150, ENEMY_TURRET  },
    {18900, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_SWARM   },
    {18900, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_SWARM   },
    {19200, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_MINIBOSS}, // SPEC.md §40.3: 1 mini-boss
    {19500, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    {19800, SPAWN_TYPE_ENEMY,   300, 130, ENEMY_CHARGER },
    {20100, SPAWN_TYPE_ENEMY,   300,  50, ENEMY_TURRET  },
    {20400, SPAWN_TYPE_POWERUP, 300,  90, POWERUP_P     },
    {20700, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_FIGHTER },
    {21000, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    {21000, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_CHARGER },
};

const StageDef stage2 = {
    21600,                                            // lengthFrames: 43,200px / 2px/frame (360s)
    2,                                                 // scrollSpeed: SPEC.md §14 normal
    stage2Events,
    sizeof(stage2Events) / sizeof(stage2Events[0]),
    { 8640, 16200 },                                   // checkpointFrames: ~40%, ~75% of 21600
    2,                                                 // bossId: Sand Worm (boss_data.c)
    2,                                                 // backgroundId: Red Desert (dune tile)
    2                                                  // musicId: Red Desert theme (audio.c)
};
