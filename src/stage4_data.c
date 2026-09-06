#include "stage.h"
#include "enemy.h"
#include "powerup.h"
#include "asteroid.h"

// SPEC.md §19/§40.5 — Asteroid Belt. Length is exact (52,800px / 3px/frame
// (SPEC.md's own "scroll normal" for this stage, faster than Stages 1-3's
// 2) = 17600 frames = 293s); GAME_SPEC.md's own stated "~5:15" duration
// doesn't match its own length+scroll numbers either (same class of
// internal inconsistency already flagged for every earlier stage). No
// 0-25/25-50/50-75/75-100% distribution table exists for Stage 4 the way
// §40.2 gave one for Stage 1 — this table's banding is a reasonable
// extrapolation, not a literal spec requirement. See PROGRESS.md for the
// actual counts vs. the spec's targets (140-180 kills, 7 enemy types, 14
// power-ups, 35-50 asteroids at ~70/25/5% small/medium/large, etc.).
static const SpawnEvent stage4Events[] = {
    // --- 0-25% (0-4400f): intro to the belt. Drone/Fighter plus small
    // asteroids to teach navigation. Max ~5 simultaneous.
    {  240, SPAWN_TYPE_ENEMY,    300,  40, ENEMY_DRONE       },
    {  360, SPAWN_TYPE_ASTEROID, 320,  70, ASTEROID_SMALL    },
    {  480, SPAWN_TYPE_ASTEROID, 340, 140, ASTEROID_SMALL    },
    {  600, SPAWN_TYPE_ENEMY,    300,  90, ENEMY_DRONE       },
    {  720, SPAWN_TYPE_ENEMY,    300,  90, ENEMY_FIGHTER     },
    {  960, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_DRONE       },
    { 1080, SPAWN_TYPE_POWERUP,  300, 100, POWERUP_P         },
    { 1200, SPAWN_TYPE_ENEMY,    300, 150, ENEMY_FIGHTER     },
    { 1320, SPAWN_TYPE_ASTEROID, 320, 100, ASTEROID_SMALL    },
    { 1560, SPAWN_TYPE_ASTEROID, 340,  50, ASTEROID_MEDIUM   },
    { 1680, SPAWN_TYPE_ENEMY,    300,  40, ENEMY_DRONE       },
    { 1800, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_FIGHTER     },
    { 1800, SPAWN_TYPE_ENEMY,    300, 160, ENEMY_DRONE       },
    { 2160, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_FIGHTER     },
    { 2640, SPAWN_TYPE_POWERUP,  300,  80, POWERUP_WIDE      },
    { 2760, SPAWN_TYPE_ENEMY,    300,  70, ENEMY_DRONE       },
    { 2880, SPAWN_TYPE_ASTEROID, 320,  40, ASTEROID_SMALL    },
    { 2880, SPAWN_TYPE_ASTEROID, 340, 150, ASTEROID_SMALL    },
    { 3120, SPAWN_TYPE_ENEMY,    300, 130, ENEMY_FIGHTER     },
    { 3600, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_TURRET      },
    { 3960, SPAWN_TYPE_ENEMY,    300,  50, ENEMY_DRONE       },
    { 4200, SPAWN_TYPE_POWERUP,  300, 100, POWERUP_1UP       }, // SPEC.md §40.5: 1 per stage

    // --- 25-50% (4400-8800f): + Bomber, large asteroids as real navigation
    // obstacles. Checkpoint at 7040f (40%). Max 8 enemies/12 projectiles/
    // ~10 asteroids (design target).
    { 4560, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_FIGHTER     },
    { 4800, SPAWN_TYPE_ASTEROID, 340,  90, ASTEROID_LARGE    },
    { 5040, SPAWN_TYPE_ENEMY,    300,  90, ENEMY_TURRET      },
    { 5400, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_BOMBER      },
    { 5640, SPAWN_TYPE_ASTEROID, 320,  40, ASTEROID_SMALL    },
    { 5640, SPAWN_TYPE_ASTEROID, 340, 160, ASTEROID_SMALL    },
    { 5880, SPAWN_TYPE_ENEMY,    300,  50, ENEMY_FIGHTER     },
    { 6000, SPAWN_TYPE_POWERUP,  300, 120, POWERUP_BOMB      },
    { 6240, SPAWN_TYPE_ENEMY,    300, 140, ENEMY_FIGHTER     },
    { 6480, SPAWN_TYPE_ASTEROID, 320,  70, ASTEROID_MEDIUM   },
    { 6720, SPAWN_TYPE_ENEMY,    300,  50, ENEMY_TURRET      },
    { 6720, SPAWN_TYPE_ENEMY,    300, 150, ENEMY_TURRET      },
    { 7080, SPAWN_TYPE_ENEMY,    300,  70, ENEMY_BOMBER      },
    { 7440, SPAWN_TYPE_ASTEROID, 320,  40, ASTEROID_SMALL    },
    { 7680, SPAWN_TYPE_POWERUP,  300,  90, POWERUP_LASER     },
    { 7920, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_TURRET      },
    { 8160, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_BOMBER      },
    { 8400, SPAWN_TYPE_ENEMY,    300, 150, ENEMY_FIGHTER     },

    // --- 50-75% (8800-13200f): Swarm groups, Shield encounters, more large
    // asteroids. Checkpoint at 13200f (75%). Max 10 enemies/18 projectiles/
    // ~15 asteroids (design target, matching §40.5's simultaneous cap).
    { 8800, SPAWN_TYPE_ASTEROID, 340, 100, ASTEROID_LARGE    },
    { 9040, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_TURRET      },
    { 9360, SPAWN_TYPE_ENEMY,    300,  40, ENEMY_SWARM       },
    { 9360, SPAWN_TYPE_ENEMY,    300,  70, ENEMY_SWARM       },
    { 9360, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_SWARM       },
    { 9680, SPAWN_TYPE_ENEMY,    300, 130, ENEMY_BOMBER      },
    {10080, SPAWN_TYPE_ASTEROID, 320,  50, ASTEROID_SMALL    },
    {10080, SPAWN_TYPE_ASTEROID, 340, 160, ASTEROID_SMALL    },
    {10320, SPAWN_TYPE_ENEMY,    300,  50, ENEMY_SWARM       },
    {10560, SPAWN_TYPE_POWERUP,  300, 100, POWERUP_SPEED     },
    {10800, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_TURRET      },
    {11040, SPAWN_TYPE_ENEMY,    300, 120, ENEMY_SHIELD      },
    {11280, SPAWN_TYPE_ENEMY,    300,  70, ENEMY_CHARGER     },
    {11520, SPAWN_TYPE_ASTEROID, 320,  70, ASTEROID_MEDIUM   },
    {11760, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_CHARGER     },
    {12000, SPAWN_TYPE_ENEMY,    300, 140, ENEMY_SWARM       },
    {12240, SPAWN_TYPE_POWERUP,  300,  90, POWERUP_HOMING    },
    {12480, SPAWN_TYPE_ENEMY,    300,  50, ENEMY_SHIELD      },
    {12720, SPAWN_TYPE_ASTEROID, 320,  40, ASTEROID_SMALL    },
    {12720, SPAWN_TYPE_ASTEROID, 340, 150, ASTEROID_SMALL    },
    {12960, SPAWN_TYPE_ENEMY,    300,  90, ENEMY_CHARGER     },

    // --- 75-100% (13200-17600f): boss preparation. Max 12 enemies/20
    // projectiles/15 asteroids (design target), mini-boss near the end.
    {13440, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_TURRET      },
    {13680, SPAWN_TYPE_ENEMY,    300,  40, ENEMY_FIGHTER     },
    {13680, SPAWN_TYPE_ENEMY,    300, 140, ENEMY_TURRET      },
    {13920, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_SWARM       },
    {14160, SPAWN_TYPE_ASTEROID, 340, 100, ASTEROID_LARGE    },
    {14400, SPAWN_TYPE_ENEMY,    300,  70, ENEMY_BOMBER      },
    {14640, SPAWN_TYPE_POWERUP,  300, 100, POWERUP_FLAME     },
    {14880, SPAWN_TYPE_ENEMY,    300, 130, ENEMY_FIGHTER     },
    {15120, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_SWARM       },
    {15120, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_SWARM       },
    {15120, SPAWN_TYPE_ASTEROID, 320,  40, ASTEROID_SMALL    },
    {15360, SPAWN_TYPE_ENEMY,    300,  50, ENEMY_TURRET      },
    {15600, SPAWN_TYPE_ENEMY,    300, 100, ENEMY_MINIBOSS    }, // SPEC.md §40.5: 1 mini-boss
    {15840, SPAWN_TYPE_ENEMY,    300,  70, ENEMY_CHARGER     },
    {16080, SPAWN_TYPE_ENEMY,    300, 150, ENEMY_SWARM       },
    {16320, SPAWN_TYPE_ENEMY,    300,  40, ENEMY_TURRET      },
    {16560, SPAWN_TYPE_POWERUP,  300,  90, POWERUP_P         },
    {16800, SPAWN_TYPE_ENEMY,    300,  90, ENEMY_BOMBER      },
    {17040, SPAWN_TYPE_ENEMY,    300,  60, ENEMY_FIGHTER     },
    {17040, SPAWN_TYPE_ENEMY,    300, 160, ENEMY_BOMBER      },
};

const StageDef stage4 = {
    17600,                                             // lengthFrames: 52,800px / 3px/frame (293s)
    3,                                                  // scrollSpeed: SPEC.md §40.5 "scroll normal" (up to 4 max, not modeled — NS-M07-005)
    stage4Events,
    sizeof(stage4Events) / sizeof(stage4Events[0]),
    { 7040, 13200 },                                    // checkpointFrames: ~40%, ~75% of 17600
    4,                                                  // bossId: Mining Fortress (boss_data.c)
    4,                                                  // backgroundId: plain starfield (asteroids are the real visual content now)
    4                                                   // musicId: Asteroid Belt theme (audio.c)
};
