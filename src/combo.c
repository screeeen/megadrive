#include "combo.h"

// Not specified exactly in SPEC.md ("se reinicia despues de un periodo sin
// destruir enemigos") — chosen here, see PROGRESS.md.
#define COMBO_TIMEOUT_FRAMES 120
#define COMBO_MAX 5

static u8 multiplier;
static u16 timeoutFrames;

void Combo_reset(void)
{
    multiplier = 1;
    timeoutFrames = 0;
}

void Combo_update(void)
{
    if (timeoutFrames == 0)
        return;

    if (--timeoutFrames == 0)
        multiplier = 1;
}

void Combo_onKill(void)
{
    if (multiplier < COMBO_MAX)
        multiplier++;

    timeoutFrames = COMBO_TIMEOUT_FRAMES;
}

u8 Combo_getMultiplier(void)
{
    return multiplier;
}
