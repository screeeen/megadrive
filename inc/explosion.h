#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_

#include <genesis.h>

// MILESTONES.md M16: "explosions" for enemy/asteroid/boss/player deaths
// and the bomb effect. A small transient-sprite pool, not a real particle
// system — no dedicated multi-frame animation exists (SGDK's sprite
// engine could support one, but no extra animation frames were drawn for
// this placeholder-era asset); the visual beat is a brief flash-and-hold
// of one sprite, matching the same level of polish as every other
// placeholder in this project. See PROGRESS.md's M16 summary.
#define EXPLOSION_POOL_SIZE 8
#define EXPLOSION_FRAMES 16 // ~0.27s on screen

typedef struct
{
    bool active;
    s16 x;
    s16 y;
    u16 timer;
    Sprite* sprite;
} Explosion;

void Explosion_poolInit(void);
void Explosion_spawn(s16 x, s16 y);
void Explosion_poolUpdate(void);
void Explosion_releaseAll(void);

#endif // _EXPLOSION_H_
