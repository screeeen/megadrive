#include "explosion.h"
#include "resources.h"

static Explosion pool[EXPLOSION_POOL_SIZE];

static void release(Explosion* e)
{
    e->active = FALSE;
    SPR_releaseSprite(e->sprite);
    e->sprite = NULL;
}

void Explosion_poolInit(void)
{
    for (u16 i = 0; i < EXPLOSION_POOL_SIZE; i++)
    {
        pool[i].active = FALSE;
        pool[i].sprite = NULL;
    }

    // Shares PAL3 with enemies/power-ups/boss/background/asteroids — see
    // PROGRESS.md's palette-budget notes; explosion.png was generated on
    // that identical palette on purpose.
}

void Explosion_spawn(s16 x, s16 y)
{
    for (u16 i = 0; i < EXPLOSION_POOL_SIZE; i++)
    {
        Explosion* e = &pool[i];

        if (e->active)
            continue;

        e->active = TRUE;
        e->x = x;
        e->y = y;
        e->timer = EXPLOSION_FRAMES;
        e->sprite = SPR_addSprite(&explosion, x, y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
        return;
    }

    // Pool full: a missed explosion is a silently dropped cosmetic
    // effect, not an error (same convention as every other pool here).
}

void Explosion_poolUpdate(void)
{
    for (u16 i = 0; i < EXPLOSION_POOL_SIZE; i++)
    {
        Explosion* e = &pool[i];

        if (!e->active)
            continue;

        // A fast blink reads as a "flash" rather than a flat sprite
        // appearing and vanishing.
        SPR_setVisibility(e->sprite, ((e->timer / 3) & 1) ? VISIBLE : HIDDEN);

        if (--e->timer == 0)
            release(e);
    }
}

void Explosion_releaseAll(void)
{
    for (u16 i = 0; i < EXPLOSION_POOL_SIZE; i++)
    {
        if (pool[i].active)
            release(&pool[i]);
    }
}
