#include "projectile.h"
#include "resources.h"

static Projectile pool[PROJECTILE_POOL_SIZE];
static ProjectileType loadedPaletteType;
static bool paletteLoaded;

static const SpriteDefinition* spriteFor(ProjectileType type)
{
    switch (type)
    {
        case PROJECTILE_TYPE_LASER:        return &bulletLaser;
        case PROJECTILE_TYPE_LASER_BIG:    return &bulletLaserBig;
        case PROJECTILE_TYPE_WIDE:         return &bulletWide;
        case PROJECTILE_TYPE_HOMING:       return &bulletHoming;
        case PROJECTILE_TYPE_FLAME:        return &bulletFlame;
        case PROJECTILE_TYPE_ENEMY_BULLET: return &enemyBullet;
        case PROJECTILE_TYPE_VULCAN:
        default:                           return &bulletVulcan;
    }
}

void Projectile_release(Projectile* p)
{
    p->active = FALSE;
    SPR_releaseSprite(p->sprite);
    p->sprite = NULL;
}

// PAL2 is reloaded per player weapon type at spawn time (only one weapon is
// ever held at once, so this is normally a no-op — see projectile.h).
// Enemy bullets are a fixed shared look on PAL3, loaded once by enemy.c
// alongside the enemy sprites themselves, since several enemies can be
// active and firing simultaneously; nothing to reload here for them.
static void ensurePalette(ProjectileType type)
{
    if (type == PROJECTILE_TYPE_ENEMY_BULLET)
        return;

    if (paletteLoaded && loadedPaletteType == type)
        return;

    PAL_setPalette(PAL2, spriteFor(type)->palette->data, DMA);
    loadedPaletteType = type;
    paletteLoaded = TRUE;
}

void Projectile_poolInit(void)
{
    for (u16 i = 0; i < PROJECTILE_POOL_SIZE; i++)
    {
        pool[i].active = FALSE;
        pool[i].sprite = NULL;
    }

    paletteLoaded = FALSE;
}

Projectile* Projectile_spawn(s16 x, s16 y, s16 vx, s16 vy, u8 damage,
                              ProjectileOwner owner, ProjectileType type,
                              s16 rangeRemaining, u8 pierceRemaining)
{
    ensurePalette(type);

    for (u16 i = 0; i < PROJECTILE_POOL_SIZE; i++)
    {
        Projectile* p = &pool[i];

        if (p->active)
            continue;

        u16 pal = (type == PROJECTILE_TYPE_ENEMY_BULLET) ? PAL3 : PAL2;

        p->active = TRUE;
        p->x = x;
        p->y = y;
        p->velocityX = vx;
        p->velocityY = vy;
        p->damage = damage;
        p->owner = owner;
        p->type = type;
        p->rangeRemaining = rangeRemaining;
        p->pierceRemaining = pierceRemaining;
        p->sprite = SPR_addSprite(spriteFor(type), x, y, TILE_ATTR(pal, TRUE, FALSE, FALSE));

        return p;
    }

    return NULL;
}

u8 Projectile_countActiveOfType(ProjectileType type)
{
    u8 count = 0;

    for (u16 i = 0; i < PROJECTILE_POOL_SIZE; i++)
    {
        if (pool[i].active && pool[i].type == type)
            count++;
    }

    return count;
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

        if (p->rangeRemaining != PROJECTILE_RANGE_UNLIMITED)
        {
            s16 traveled = abs(p->velocityX) + abs(p->velocityY);
            p->rangeRemaining -= traveled;

            if (p->rangeRemaining <= 0)
            {
                Projectile_release(p);
                continue;
            }
        }

        if (p->x < -PROJECTILE_SPRITE_W || p->x > 320 ||
            p->y < -PROJECTILE_SPRITE_H || p->y > 224)
        {
            Projectile_release(p);
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
            Projectile_release(&pool[i]);
    }
}

Projectile* Projectile_getPool(void)
{
    return pool;
}
