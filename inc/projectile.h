#ifndef _PROJECTILE_H_
#define _PROJECTILE_H_

#include <genesis.h>

#define PROJECTILE_POOL_SIZE 16
#define PROJECTILE_SPRITE_W  8
#define PROJECTILE_SPRITE_H  8

typedef enum
{
    PROJECTILE_OWNER_PLAYER,
    PROJECTILE_OWNER_ENEMY
} ProjectileOwner;

typedef enum
{
    PROJECTILE_TYPE_VULCAN
} ProjectileType;

typedef struct
{
    bool active;
    s16 x;
    s16 y;
    s16 velocityX;
    s16 velocityY;
    u8 damage;
    ProjectileOwner owner;
    ProjectileType type;
    Sprite* sprite;
} Projectile;

void Projectile_poolInit(void);

// Fixed-size static pool, no gameplay-time allocation (AGENT.md §8). Returns
// the spawned slot, or NULL if the pool is full — SPEC.md's projectile
// limits are a design budget, so a full pool just silently drops the shot
// rather than being treated as an error.
Projectile* Projectile_spawn(s16 x, s16 y, s16 vx, s16 vy, u8 damage,
                              ProjectileOwner owner, ProjectileType type);

// Advances all active projectiles and releases (deactivates + frees the
// sprite of) any that left the visible screen.
void Projectile_poolUpdate(void);

// Deactivates and releases every active projectile's sprite immediately.
// Called on GAME_OVER so a mid-flight volley doesn't leak hardware sprites
// into the next game (mirrors Player's own GAME_OVER cleanup).
void Projectile_releaseAll(void);

#endif // _PROJECTILE_H_
