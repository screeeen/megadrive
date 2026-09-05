#ifndef _PROJECTILE_H_
#define _PROJECTILE_H_

#include <genesis.h>

#define PROJECTILE_POOL_SIZE 16
#define PROJECTILE_SPRITE_W  8
#define PROJECTILE_SPRITE_H  8

// -1 in rangeRemaining means "unlimited range" (removed only by leaving the
// screen). PROJECTILE_PIERCE_NONE means "destroyed on first hit" once M05
// wires real collisions.
#define PROJECTILE_RANGE_UNLIMITED (-1)
#define PROJECTILE_PIERCE_NONE     0

typedef enum
{
    PROJECTILE_OWNER_PLAYER,
    PROJECTILE_OWNER_ENEMY
} ProjectileOwner;

typedef enum
{
    PROJECTILE_TYPE_VULCAN,
    PROJECTILE_TYPE_LASER,
    PROJECTILE_TYPE_LASER_BIG, // Laser at weapon L3: SPEC.md §7 "visually larger but technically cheap"
    PROJECTILE_TYPE_WIDE,
    PROJECTILE_TYPE_HOMING,
    PROJECTILE_TYPE_FLAME
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

    // SPEC.md §7: Flame is short-range (counts down in pixels of travel),
    // Laser pierces small enemies (counts down hits, once M05 wires real
    // collisions).
    s16 rangeRemaining;
    u8 pierceRemaining;

    Sprite* sprite;
} Projectile;

void Projectile_poolInit(void);

// Fixed-size static pool, no gameplay-time allocation (AGENT.md §8). Returns
// the spawned slot, or NULL if the pool is full — SPEC.md's projectile
// limits are a design budget, so a full pool just silently drops the shot
// rather than being treated as an error.
//
// Only one hardware palette line (PAL2) is free for all 5 projectile types
// combined (PAL0=text, PAL1=player, PAL3=debug hitbox) — see PROGRESS.md's
// palette-budget note. Spawning reloads PAL2 for the given type if it
// isn't already loaded; since only one weapon is ever held at a time this
// is normally a no-op, but a volley left over from a weapon just switched
// away from will render in the new weapon's colors until it despawns. A
// known, harmless placeholder-era cosmetic quirk, not a bug.
Projectile* Projectile_spawn(s16 x, s16 y, s16 vx, s16 vy, u8 damage,
                              ProjectileOwner owner, ProjectileType type,
                              s16 rangeRemaining, u8 pierceRemaining);

// Number of currently-active projectiles of a given type (e.g. SPEC.md §7's
// Homing active-missile limit).
u8 Projectile_countActiveOfType(ProjectileType type);

// Advances all active projectiles and releases (deactivates + frees the
// sprite of) any that left the visible screen or ran out of range.
void Projectile_poolUpdate(void);

// Deactivates and releases every active projectile's sprite immediately.
// Called on GAME_OVER so a mid-flight volley doesn't leak hardware sprites
// into the next game (mirrors Player's own GAME_OVER cleanup).
void Projectile_releaseAll(void);

#endif // _PROJECTILE_H_
