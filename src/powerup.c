#include "powerup.h"
#include "resources.h"

static Powerup pool[POWERUP_POOL_SIZE];

static const SpriteDefinition* spriteFor(PowerupType type)
{
    switch (type)
    {
        case POWERUP_WIDE:   return &powerupWide;
        case POWERUP_HOMING: return &powerupHoming;
        case POWERUP_FLAME:  return &powerupFlame;
        case POWERUP_P:      return &powerupP;
        case POWERUP_BOMB:   return &powerupBomb;
        case POWERUP_SPEED:  return &powerupSpeed;
        case POWERUP_1UP:    return &powerup1up;
        case POWERUP_LASER:
        default:             return &powerupLaser;
    }
}

void Powerup_release(Powerup* p)
{
    p->active = FALSE;
    SPR_releaseSprite(p->sprite);
    p->sprite = NULL;
}

void Powerup_poolInit(void)
{
    for (u16 i = 0; i < POWERUP_POOL_SIZE; i++)
    {
        pool[i].active = FALSE;
        pool[i].sprite = NULL;
    }

    // Shares the enemy palette (PAL3) — see PROGRESS.md's palette-budget
    // notes; all power-up placeholders were generated on that identical
    // palette on purpose.
}

Powerup* Powerup_spawn(PowerupType type, s16 x, s16 y)
{
    for (u16 i = 0; i < POWERUP_POOL_SIZE; i++)
    {
        Powerup* p = &pool[i];

        if (p->active)
            continue;

        p->active = TRUE;
        p->type = type;
        p->x = x;
        p->y = y;
        p->sprite = SPR_addSprite(spriteFor(type), x, y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));

        return p;
    }

    return NULL;
}

void Powerup_poolUpdate(void)
{
    for (u16 i = 0; i < POWERUP_POOL_SIZE; i++)
    {
        Powerup* p = &pool[i];

        if (!p->active)
            continue;

        p->x += POWERUP_DRIFT_SPEED;
        p->y += POWERUP_FALL_SPEED;

        if (p->x < -POWERUP_SIZE || p->y > 224)
        {
            Powerup_release(p);
            continue;
        }

        SPR_setPosition(p->sprite, p->x, p->y);
    }
}

void Powerup_releaseAll(void)
{
    for (u16 i = 0; i < POWERUP_POOL_SIZE; i++)
    {
        if (pool[i].active)
            Powerup_release(&pool[i]);
    }
}

Powerup* Powerup_getPool(void)
{
    return pool;
}
