#include "spawn_manager.h"
#include "enemy.h"
#include "powerup.h"

static const StageDef* currentStage;
static u32 currentFrame;
static u16 cursor;
static u8 checkpointsPassed;

static void resetCursorAt(u32 frame)
{
    currentFrame = frame;
    cursor = 0;

    while (cursor < currentStage->spawnEventCount &&
           currentStage->spawnEvents[cursor].frame < currentFrame)
        cursor++;

    checkpointsPassed = 0;

    for (u8 i = 0; i < 2; i++)
    {
        if (currentFrame >= currentStage->checkpointFrames[i])
            checkpointsPassed = (u8) (i + 1);
    }
}

void SpawnManager_start(const StageDef* stage)
{
    currentStage = stage;
    resetCursorAt(0);
}

void SpawnManager_resumeFromCheckpoint(const StageDef* stage, u8 checkpoints)
{
    currentStage = stage;

    u32 resumeFrame = 0;

    if (checkpoints > 0)
        resumeFrame = stage->checkpointFrames[checkpoints - 1];

    resetCursorAt(resumeFrame);
}

void SpawnManager_update(void)
{
    currentFrame++;

    while (cursor < currentStage->spawnEventCount &&
           currentStage->spawnEvents[cursor].frame <= currentFrame)
    {
        const SpawnEvent* e = &currentStage->spawnEvents[cursor];

        if (e->type == SPAWN_TYPE_ENEMY)
            Enemy_spawn((EnemyType) e->variant, e->x, e->y);
        else
            Powerup_spawn((PowerupType) e->variant, e->x, e->y);

        cursor++;
    }

    for (u8 i = checkpointsPassed; i < 2; i++)
    {
        if (currentFrame >= currentStage->checkpointFrames[i])
            checkpointsPassed = (u8) (i + 1);
    }
}

bool SpawnManager_isComplete(void)
{
    return currentFrame >= currentStage->lengthFrames;
}

u8 SpawnManager_getCheckpointsPassed(void)
{
    return checkpointsPassed;
}
