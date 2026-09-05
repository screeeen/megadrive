#include "input.h"

static InputState state;

void Input_init(void)
{
    JOY_init();
}

void Input_update(void)
{
    u16 value = JOY_readJoypad(JOY_1);

    state.up    = (value & BUTTON_UP)    != 0;
    state.down  = (value & BUTTON_DOWN)  != 0;
    state.left  = (value & BUTTON_LEFT)  != 0;
    state.right = (value & BUTTON_RIGHT) != 0;
}

const InputState* Input_getState(void)
{
    return &state;
}
