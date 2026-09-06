#ifndef _SPAWN_MANAGER_H_
#define _SPAWN_MANAGER_H_

#include <genesis.h>
#include "stage.h"

// Fresh run of a stage, starting at frame 0.
void SpawnManager_start(const StageDef* stage);

// Rewinds to a checkpoint (0 = stage start, 1 = checkpointFrames[0], 2 =
// checkpointFrames[1]) and recomputes which spawn events are still ahead
// of it. Used on respawn after death and on continue (SPEC.md §20/§26).
void SpawnManager_resumeFromCheckpoint(const StageDef* stage, u8 checkpointsPassed);

// Call once per GAME frame: advances the frame counter, fires any events
// whose frame has arrived (spawning into Enemy/Powerup's own pools, which
// already respect their own size limits), and updates the checkpoint
// counter.
void SpawnManager_update(void);

bool SpawnManager_isComplete(void);
u8 SpawnManager_getCheckpointsPassed(void);

// M14 HUD: how far into the stage's timeline the current frame is, for a
// "time/progress remaining" readout.
u32 SpawnManager_getCurrentFrame(void);

#endif // _SPAWN_MANAGER_H_
