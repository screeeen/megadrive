#include "input.h"

static InputState state;
static u16 previousValue;

void Input_init(void)
{
    JOY_init();
    previousValue = 0;
}

void Input_update(void)
{
    u16 value = JOY_readJoypad(JOY_1);

    state.up    = (value & BUTTON_UP)    != 0;
    state.down  = (value & BUTTON_DOWN)  != 0;
    state.left  = (value & BUTTON_LEFT)  != 0;
    state.right = (value & BUTTON_RIGHT) != 0;
    state.start = (value & BUTTON_START) != 0;
    state.startPressed = state.start && !(previousValue & BUTTON_START);
    state.a = (value & BUTTON_A) != 0;
    state.b = (value & BUTTON_B) != 0;
    state.bPressed = state.b && !(previousValue & BUTTON_B);
    state.cPressed = (value & BUTTON_C) && !(previousValue & BUTTON_C);

    previousValue = value;
}

const InputState* Input_getState(void)
{
    return &state;
}
