#ifndef _ENEMY_H_
#define _ENEMY_H_

#include <genesis.h>
#include "enemy_logic.h"

// SPEC.md §24: never exceed 12 active enemies without explicit validation.
#define ENEMY_POOL_SIZE 12

typedef enum
{
    ENEMY_DRONE,
    ENEMY_FIGHTER,
    ENEMY_BOMBER,
    ENEMY_TURRET,
    ENEMY_SWARM,
    ENEMY_CHARGER,
    ENEMY_SHIELD,
    ENEMY_MINIBOSS, // SPEC.md §40.2: one per stage, tougher than any regular type
    ENEMY_TYPE_COUNT
} EnemyType;

typedef enum
{
    ENEMY_STATE_ACTIVE,
    ENEMY_STATE_PAUSED,   // Charger only: stopped, about to aim+charge
    ENEMY_STATE_CHARGING, // Charger only: mid-dash
    ENEMY_STATE_DEAD
} EnemyState;

typedef struct
{
    bool active;
    EnemyType type;
    EnemyState state;

    s16 x;
    s16 y;
    s16 velocityX;
    s16 velocityY;
    u8 spriteW; // Bomber/Miniboss are 16x16; every other type is 8x8
    u8 spriteH;

    s16 hp;
    s16 maxHp;
    u16 scoreValue;

    u16 shootTimer;
    u16 stateTimer; // generic per-type countdown (Charger's pause/charge phases)
    bool vulnerable; // Shield only: false while its frontal armor blocks damage

    Sprite* sprite;
} Enemy;

void Enemy_poolInit(void);

// Fixed-size static pool (AGENT.md §8), mirroring projectile.c. Returns the
// spawned slot, or NULL if already at the 12-enemy budget.
Enemy* Enemy_spawn(EnemyType type, s16 x, s16 y);

// Advances every active enemy (movement, timers, firing) given the
// player's current position (for aimed shots / Charger's target line).
void Enemy_poolUpdate(s16 playerX, s16 playerY);

// Applies damage to one enemy (see enemy_logic.h). If it dies, releases its
// sprite, deactivates it, and returns its score value; otherwise returns 0.
// A Shield enemy currently not vulnerable ignores the hit entirely (also
// returns 0).
u16 Enemy_hit(Enemy* enemy, u8 damage);

void Enemy_releaseAll(void);

Enemy* Enemy_getPool(void);

#endif // _ENEMY_H_
