#include <genesis.h>
#include "input.h"
#include "player.h"
#include "audio.h"

static Player player;

int main(bool hardReset)
{
    VDP_drawText("HELLO WORLD!", 10, 12);

    SPR_init();
    Input_init();
    Audio_init();
    Player_init(&player, 144, 96);

    while (TRUE)
    {
        Input_update();
        Player_update(&player, Input_getState());
        Audio_update();

        SPR_update();
        SYS_doVBlankProcess();
    }

    return 0;
}
