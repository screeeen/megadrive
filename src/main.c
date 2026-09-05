#include <genesis.h>
#include "game_state.h"
#include "input.h"

int main(bool hardReset)
{
    SPR_init();
    Input_init();
    GameState_init();

    while (TRUE)
    {
        GameState_update();
        SYS_doVBlankProcess();
    }

    return 0;
}
