#include "stage.h"
#include "enemy.h"
#include "powerup.h"

// SPEC.md §16/§40.2 — Orbital City. Length is exact (38,400px / 2px/frame =
// 19200 frames = 320s); pacing follows the documented 0-25/25-50/50-75/
// 75-100% density bands. This hand-authored table is a genuine, honest
// placeholder for that pacing, NOT a claim of exact compliance with every
// numeric target (90-120 kills, exactly 12 power-ups, etc.) — see
// PROGRESS.md for the actual counts this table produces vs. the spec's
// targets. Real content authoring (and almost certainly tooling to
// generate/tune tables like this one) is beyond what a single hand-typed
// C array should be trusted to nail exactly.
static const SpawnEvent stage1Events[] = {
    // --- 0-25% (0-4800f): intro. Drone/Fighter only, simple formations,
    // teaches movement/shooting, one early power-up. Max ~5 simultaneous.
    {  240, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_DRONE   },
    {  360, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_DRONE   },
    {  480, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_DRONE   },
    {  840, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_P     },
    { 1200, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    { 1320, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_FIGHTER },
    { 1800, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_DRONE   },
    { 1800, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_DRONE   },
    { 1800, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_DRONE   },
    { 2400, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_FIGHTER },
    { 3000, SPAWN_TYPE_POWERUP, 300,  80, POWERUP_WIDE  },
    { 3600, SPAWN_TYPE_ENEMY,   300,  50, ENEMY_DRONE   },
    { 3600, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_FIGHTER },
    { 3600, SPAWN_TYPE_ENEMY,   300, 150, ENEMY_DRONE   },
    { 4800, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_1UP   }, // SPEC.md §40.2: 1 per stage

    // --- 25-50% (4800-9600f): + Turret, first bursts. Max 8 enemies/12 bullets.
    { 5400, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_TURRET  },
    { 6000, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    { 6000, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_FIGHTER },
    { 6600, SPAWN_TYPE_POWERUP, 300, 120, POWERUP_BOMB  },
    { 7200, SPAWN_TYPE_ENEMY,   300, 120, ENEMY_TURRET  },
    { 7800, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_DRONE   },
    { 7800, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_DRONE   },
    { 7800, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_DRONE   },
    { 8400, SPAWN_TYPE_ENEMY,   300,  80, ENEMY_FIGHTER },
    { 8400, SPAWN_TYPE_ENEMY,   300, 150, ENEMY_TURRET  },
    { 9000, SPAWN_TYPE_POWERUP, 300,  90, POWERUP_LASER },
    { 9600, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    { 9600, SPAWN_TYPE_ENEMY,   300, 170, ENEMY_TURRET  },

    // --- 50-75% (9600-14400f): Fighter+Turret combos, Swarm groups,
    // power-ups. Max 10 enemies/18 bullets.
    { 10200, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_SWARM   },
    { 10200, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_SWARM   },
    { 10200, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },
    { 10800, SPAWN_TYPE_ENEMY,   300, 130, ENEMY_TURRET  },
    { 11400, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_SPEED },
    { 12000, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    { 12000, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_FIGHTER },
    { 12000, SPAWN_TYPE_ENEMY,   300, 110, ENEMY_TURRET  },
    { 12600, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_SWARM   },
    { 12600, SPAWN_TYPE_ENEMY,   300,  70, ENEMY_SWARM   },
    { 12600, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },
    { 13200, SPAWN_TYPE_POWERUP, 300,  90, POWERUP_HOMING},
    { 13800, SPAWN_TYPE_ENEMY,   300,  50, ENEMY_FIGHTER },
    { 13800, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_TURRET  },

    // --- 75-100% (14400-19200f): boss preparation. Max 12 enemies/20
    // bullets, mini-boss near the end of this band.
    { 14400, SPAWN_TYPE_ENEMY,   300,  40, ENEMY_DRONE   },
    { 14400, SPAWN_TYPE_ENEMY,   300,  90, ENEMY_FIGHTER },
    { 14400, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_TURRET  },
    { 15000, SPAWN_TYPE_POWERUP, 300, 100, POWERUP_FLAME },
    { 15600, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_SWARM   },
    { 15600, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_SWARM   },
    { 15600, SPAWN_TYPE_ENEMY,   300, 140, ENEMY_SWARM   },
    { 16200, SPAWN_TYPE_ENEMY,   300, 100, ENEMY_MINIBOSS}, // SPEC.md §40.2: 1 mini-boss
    { 17400, SPAWN_TYPE_POWERUP, 300,  90, POWERUP_P     },
    { 18000, SPAWN_TYPE_ENEMY,   300,  60, ENEMY_FIGHTER },
    { 18000, SPAWN_TYPE_ENEMY,   300, 160, ENEMY_FIGHTER },
};

const StageDef stage1 = {
    19200,                                           // lengthFrames: 38,400px / 2px/frame (320s)
    2,                                               // scrollSpeed: SPEC.md §14 normal
    stage1Events,
    sizeof(stage1Events) / sizeof(stage1Events[0]),
    { 7680, 14400 },                                 // checkpointFrames: ~40%, ~75% of 19200
    1,                                                // bossId: Orbital Guardian (game_state.c's only boss so far)
    1,                                                // backgroundId: Orbital City (space + buildings placeholder)
    1                                                 // musicId: Orbital City theme (audio.c)
};
