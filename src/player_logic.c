#include "player_logic.h"

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
