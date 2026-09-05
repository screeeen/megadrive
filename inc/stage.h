#ifndef _STAGE_H_
#define _STAGE_H_

#include <genesis.h>

#define SPAWN_TYPE_ENEMY   0
#define SPAWN_TYPE_POWERUP 1

// TASKS.md NS-M07-001. `variant` holds an EnemyType or PowerupType value,
// depending on `type` — a single byte can't hold both enums directly since
// their numeric ranges overlap.
typedef struct
{
    u32 frame; // frame offset from stage start when this fires
    u8 type;   // SPAWN_TYPE_*
    s16 x;
    s16 y;
    u8 variant;
} SpawnEvent;

// TASKS.md NS-M07-002. bossId/backgroundId/musicId are placeholders for
// M09 (Boss Framework)/M08 (art)/M15 (Audio) to give real meaning to —
// this milestone only needs the data shape to exist, not working systems
// behind every field yet.
typedef struct
{
    u32 lengthFrames;
    u8 scrollSpeed; // SPEC.md §14: 2 normal, up to 4 max px/frame
    const SpawnEvent* spawnEvents;
    u16 spawnEventCount;
    u32 checkpointFrames[2]; // SPEC.md §20: ~40%, ~75% of lengthFrames
    u8 bossId;
    u8 backgroundId;
    u8 musicId;
} StageDef;

// M07's generic-system placeholder — kept as a lightweight, fast-to-loop
// sanity check distinct from the real (much longer) Stage 1.
extern const StageDef testStage;

// SPEC.md §16 Orbital City, M08. Real length/pacing/mini-boss per §40.2 —
// see stage1_data.c and PROGRESS.md for exactly which numeric targets a
// hand-authored placeholder table does and doesn't hit.
extern const StageDef stage1;

#endif // _STAGE_H_
