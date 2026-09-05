#include "boss_logic.h"

BossHitResult Boss_applyDamage(BossHitState state, short maxHp, unsigned char damage)
{
    BossHitResult result;
    result.phaseChanged = false;
    result.destroyed = false;

    short newHp = (short) (state.hp - damage);
    if (newHp < 0)
        newHp = 0;

    result.state.hp = newHp;
    result.state.phase = state.phase;

    if (newHp == 0)
    {
        result.destroyed = true;
        return result;
    }

    unsigned char newPhase = 1;

    if (newHp <= maxHp / 3)
        newPhase = 3;
    else if (newHp <= (short) ((maxHp * 2) / 3))
        newPhase = 2;

    if (newPhase != state.phase)
    {
        result.state.phase = newPhase;
        result.phaseChanged = true;
    }

    return result;
}
