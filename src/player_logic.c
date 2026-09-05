#include "player_logic.h"

WeaponState Weapon_switchNext(WeaponState state)
{
    WeaponState result;
    result.weapon = (WeaponType) ((state.weapon + 1) % WEAPON_COUNT);
    result.weaponLevel = 1;
    return result;
}

WeaponState Weapon_levelUp(WeaponState state)
{
    WeaponState result = state;

    if (result.weaponLevel < 3)
        result.weaponLevel++;

    return result;
}

PlayerHitResult Player_applyHit(PlayerHitState state)
{
    PlayerHitResult result;
    result.state = state;
    result.lifeLost = false;
    result.gameOver = false;

    if (state.weaponLevel > 1)
    {
        result.state.weaponLevel = (unsigned char) (state.weaponLevel - 1);
        return result;
    }

    result.lifeLost = true;
    result.state.weaponLevel = 1;

    if (state.lives > 0)
        result.state.lives = (unsigned char) (state.lives - 1);

    if (result.state.lives == 0)
        result.gameOver = true;

    return result;
}
