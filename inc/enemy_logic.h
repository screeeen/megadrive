#ifndef _ENEMY_LOGIC_H_
#define _ENEMY_LOGIC_H_

#include <stdbool.h>

// Hardware-free, like player_logic.h / game_state_logic.h.

typedef struct
{
    short hp;
} EnemyHitState;

typedef struct
{
    EnemyHitState state;
    bool destroyed;
} EnemyHitResult;

// SPEC.md §11/§12: damage is subtracted; hp never goes negative, and
// destroyed becomes true once it reaches 0. Pure decision, no side effects
// — the caller applies the result and drives sprite/score/state effects.
EnemyHitResult Enemy_applyDamage(EnemyHitState state, unsigned char damage);

#endif // _ENEMY_LOGIC_H_
