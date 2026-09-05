#include "projectile.h"
#include "resources.h"

static Projectile pool[PROJECTILE_POOL_SIZE];

static void release(Projectile* p)
{
    p->active = FALSE;
    SPR_releaseSprite(p->sprite);
    p->sprite = NULL;
}

void Projectile_poolInit(void)
{
    for (u16 i = 0; i < PROJECTILE_POOL_SIZE; i++)
    {
        pool[i].active = FALSE;
        pool[i].sprite = NULL;
    }

    PAL_setPalette(PAL2, bulletVulcan.palette->data, DMA);
}

Projectile* Projectile_spawn(s16 x, s16 y, s16 vx, s16 vy, u8 damage,
                              ProjectileOwner owner, ProjectileType type)
{
    for (u16 i = 0; i < PROJECTILE_POOL_SIZE; i++)
    {
        Projectile* p = &pool[i];

        if (p->active)
            continue;

        p->active = TRUE;
        p->x = x;
        p->y = y;
        p->velocityX = vx;
        p->velocityY = vy;
        p->damage = damage;
        p->owner = owner;
        p->type = type;
        p->sprite = SPR_addSprite(&bulletVulcan, x, y, TILE_ATTR(PAL2, TRUE, FALSE, FALSE));

        return p;
    }

    return NULL;
}

void Projectile_poolUpdate(void)
{
    for (u16 i = 0; i < PROJECTILE_POOL_SIZE; i++)
    {
        Projectile* p = &pool[i];

        if (!p->active)
            continue;

        p->x += p->velocityX;
        p->y += p->velocityY;

        if (p->x < -PROJECTILE_SPRITE_W || p->x > 320 ||
            p->y < -PROJECTILE_SPRITE_H || p->y > 224)
        {
            release(p);
            continue;
        }

        SPR_setPosition(p->sprite, p->x, p->y);
    }
}

void Projectile_releaseAll(void)
{
    for (u16 i = 0; i < PROJECTILE_POOL_SIZE; i++)
    {
        if (pool[i].active)
            release(&pool[i]);
    }
}
