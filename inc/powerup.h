#ifndef _POWERUP_H_
#define _POWERUP_H_

#include <genesis.h>

// Concurrent on-screen power-ups are always a handful, well under the
// enemy/projectile budgets (SPEC.md §24 doesn't give this one a number).
#define POWERUP_POOL_SIZE 8
#define POWERUP_SIZE      8
#define POWERUP_FALL_SPEED 1
#define POWERUP_DRIFT_SPEED (-1) // drifts slowly toward the player, like everything else

typedef enum
{
    POWERUP_LASER,
    POWERUP_WIDE,
    POWERUP_HOMING,
    POWERUP_FLAME,
    POWERUP_P,     // weapon level +1
    POWERUP_BOMB,  // bomb +1
    POWERUP_SPEED, // temporary speed increase
    POWERUP_1UP,   // life +1
    POWERUP_TYPE_COUNT
} PowerupType;

typedef struct
{
    bool active;
    PowerupType type;
    s16 x;
    s16 y;
    Sprite* sprite;
} Powerup;

void Powerup_poolInit(void);
Powerup* Powerup_spawn(PowerupType type, s16 x, s16 y);

// Slow, predictable fall+drift (SPEC.md §10); releases anything that
// leaves the screen.
void Powerup_poolUpdate(void);

void Powerup_releaseAll(void);
void Powerup_release(Powerup* p);
Powerup* Powerup_getPool(void);

#endif // _POWERUP_H_
