#include "enemy_logic.h"

EnemyHitResult Enemy_applyDamage(EnemyHitState state, unsigned char damage)
{
    EnemyHitResult result;

    if (damage >= state.hp)
    {
        result.state.hp = 0;
        result.destroyed = true;
    }
    else
    {
        result.state.hp = (short) (state.hp - damage);
        result.destroyed = false;
    }

    return result;
}
