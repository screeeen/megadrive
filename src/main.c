#include <genesis.h>

int main(bool hardReset)
{
    VDP_drawText("HELLO WORLD!", 10, 12);

    while (TRUE)
    {
        SYS_doVBlankProcess();
    }

    return 0;
}
