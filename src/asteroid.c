#include "asteroid.h"
#include "resources.h"

// SPEC.md §19 gives the destructible/indestructible/size split but no
// speed/HP/score numbers — chosen here (see PROGRESS.md), same pattern as
// every other placeholder-era enemy/power-up constant.
#define SMALL_SPEED  -3
#define SMALL_HP      1
#define SMALL_SCORE  50

#define MEDIUM_SPEED -2
#define MEDIUM_HP     2
#define MEDIUM_SCORE 100

#define LARGE_SPEED  -1

static Asteroid pool[ASTEROID_POOL_SIZE];

static const SpriteDefinition* spriteFor(AsteroidSize size)
{
    switch (size)
    {
        case ASTEROID_MEDIUM: return &asteroidMedium;
        case ASTEROID_LARGE:  return &asteroidLarge;
        case ASTEROID_SMALL:
        default:              return &asteroidSmall;
    }
}

static void release(Asteroid* a)
{
    a->active = FALSE;
    SPR_releaseSprite(a->sprite);
    a->sprite = NULL;
}

void Asteroid_poolInit(void)
{
    for (u16 i = 0; i < ASTEROID_POOL_SIZE; i++)
    {
        pool[i].active = FALSE;
        pool[i].sprite = NULL;
    }

    // Shares PAL3 with enemies/power-ups/boss/background — see
    // PROGRESS.md's palette-budget notes; the asteroid placeholders were
    // generated on that identical palette on purpose.
}

Asteroid* Asteroid_spawn(AsteroidSize size, s16 x, s16 y)
{
    for (u16 i = 0; i < ASTEROID_POOL_SIZE; i++)
    {
        Asteroid* a = &pool[i];

        if (a->active)
            continue;

        a->active = TRUE;
        a->x = x;
        a->y = y;
        a->size = size;

        switch (size)
        {
            case ASTEROID_MEDIUM:
                a->velocityX = MEDIUM_SPEED;
                a->hp = MEDIUM_HP;
                a->scoreValue = MEDIUM_SCORE;
                a->spriteW = 16;
                a->spriteH = 16;
                break;

            case ASTEROID_LARGE:
                a->velocityX = LARGE_SPEED;
                a->hp = 0; // indestructible — Asteroid_hit never applies damage to it
                a->scoreValue = 0;
                a->spriteW = 24;
                a->spriteH = 24;
                break;

            case ASTEROID_SMALL:
            default:
                a->velocityX = SMALL_SPEED;
                a->hp = SMALL_HP;
                a->scoreValue = SMALL_SCORE;
                a->spriteW = 8;
                a->spriteH = 8;
                break;
        }

        a->sprite = SPR_addSprite(spriteFor(size), x, y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));

        return a;
    }

    return NULL;
}

void Asteroid_poolUpdate(void)
{
    for (u16 i = 0; i < ASTEROID_POOL_SIZE; i++)
    {
        Asteroid* a = &pool[i];

        if (!a->active)
            continue;

        a->x += a->velocityX;

        if (a->x < -a->spriteW)
        {
            release(a);
            continue;
        }

        SPR_setPosition(a->sprite, a->x, a->y);
    }
}

void Asteroid_releaseAll(void)
{
    for (u16 i = 0; i < ASTEROID_POOL_SIZE; i++)
    {
        if (pool[i].active)
            release(&pool[i]);
    }
}

Asteroid* Asteroid_getPool(void)
{
    return pool;
}

u16 Asteroid_hit(Asteroid* asteroid, u8 damage)
{
    if (asteroid->size == ASTEROID_LARGE)
        return 0; // SPEC.md §19: indestructible

    if (damage >= asteroid->hp)
    {
        u16 score = asteroid->scoreValue;
        release(asteroid);
        return score;
    }

    asteroid->hp -= damage;
    return 0;
}
