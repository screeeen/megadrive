#include "stage.h"
#include "enemy.h"
#include "powerup.h"

// Placeholder content: every enemy type at least once, two deliberate
// formations (a 2-Fighter pair, a 3-Swarm group) to demonstrate
// NS-M07-003's "spawn enemies"/formations via data rather than code, and
// checkpoints at the spec-mandated ~40%/~75% marks. Not stage-design work
// (M08's job) — just enough to prove the SpawnEvent/StageDef/SpawnManager
// system actually drives real spawns from data.
static const SpawnEvent testStageEvents[] = {
    {  60, SPAWN_TYPE_ENEMY, 300,  20, ENEMY_DRONE   },
    { 150, SPAWN_TYPE_ENEMY, 300,  45, ENEMY_FIGHTER },
    { 150, SPAWN_TYPE_ENEMY, 300,  70, ENEMY_FIGHTER }, // formation: 2 Fighters, same frame
    { 240, SPAWN_TYPE_ENEMY, 300,  95, ENEMY_BOMBER  },
    { 330, SPAWN_TYPE_ENEMY, 300, 120, ENEMY_TURRET  },
    { 420, SPAWN_TYPE_ENEMY, 300, 145, ENEMY_SWARM   },
    { 420, SPAWN_TYPE_ENEMY, 300, 170, ENEMY_SWARM   },
    { 420, SPAWN_TYPE_ENEMY, 300, 195, ENEMY_SWARM   }, // formation: 3-Swarm group
    { 510, SPAWN_TYPE_ENEMY, 300,  60, ENEMY_CHARGER },
    { 600, SPAWN_TYPE_ENEMY, 300, 100, ENEMY_SHIELD  },
    { 690, SPAWN_TYPE_ENEMY, 300,  20, ENEMY_DRONE   },
    { 690, SPAWN_TYPE_ENEMY, 300, 180, ENEMY_DRONE   },
    { 780, SPAWN_TYPE_ENEMY, 300,  45, ENEMY_FIGHTER },
    { 870, SPAWN_TYPE_ENEMY, 300,  95, ENEMY_BOMBER  },
    { 960, SPAWN_TYPE_ENEMY, 300, 120, ENEMY_TURRET  },
};

const StageDef testStage = {
    1200,                                             // lengthFrames: 20s at 60Hz
    2,                                                // scrollSpeed: SPEC.md §14 normal
    testStageEvents,
    sizeof(testStageEvents) / sizeof(testStageEvents[0]),
    { 480, 900 },                                     // checkpointFrames: ~40%, ~75% of 1200
    0,                                                 // bossId (M09)
    0,                                                 // backgroundId (M08)
    0                                                  // musicId (M15)
};
