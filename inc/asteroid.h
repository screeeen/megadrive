#ifndef _ASTEROID_H_
#define _ASTEROID_H_

#include <genesis.h>

// SPEC.md §40.5: "15 obstaculos simultaneos" max.
#define ASTEROID_POOL_SIZE 15

// SPEC.md §19: "algunos asteroides pueden destruirse, otros son
// indestructibles" — not tied to size explicitly in SPEC.md, but tying it
// to size (small/medium destructible, large indestructible) is the
// natural, common reading and needs no extra field. See PROGRESS.md's M11
// summary.
typedef enum
{
    ASTEROID_SMALL,  // destructible, 1 HP
    ASTEROID_MEDIUM, // destructible, 2 HP
    ASTEROID_LARGE   // indestructible — a navigation obstacle (SPEC.md §40.5)
} AsteroidSize;

typedef struct
{
    bool active;
    s16 x;
    s16 y;
    s16 velocityX;
    AsteroidSize size;
    u8 hp;
    u16 scoreValue;
    u8 spriteW;
    u8 spriteH;
    Sprite* sprite;
} Asteroid;

void Asteroid_poolInit(void);

// Fixed-size static pool, no gameplay-time allocation (AGENT.md §8).
// Returns the spawned slot, or NULL if the pool is full.
Asteroid* Asteroid_spawn(AsteroidSize size, s16 x, s16 y);

// Advances all active asteroids and releases any that left the screen.
void Asteroid_poolUpdate(void);

void Asteroid_releaseAll(void);

// Direct access to the pool for collision resolution (combat.c).
Asteroid* Asteroid_getPool(void);

// Applies damage; large (indestructible) asteroids always return 0 and
// are never destroyed, matching SPEC.md §19's "otros son indestructibles".
u16 Asteroid_hit(Asteroid* asteroid, u8 damage);

#endif // _ASTEROID_H_
