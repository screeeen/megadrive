#include "game_state_logic.h"

GameState GameState_computeNext(GameState current, bool startPressed)
{
    if (!startPressed)
        return current;

    switch (current)
    {
        case STATE_TITLE:
            return STATE_GAME;
        case STATE_GAME:
            return STATE_PAUSE;
        case STATE_PAUSE:
            return STATE_GAME;
        default:
            return current;
    }
}
