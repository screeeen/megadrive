#ifndef _INPUT_H_
#define _INPUT_H_

#include <genesis.h>

typedef struct
{
    bool up;
    bool down;
    bool left;
    bool right;
} InputState;

void Input_init(void);
void Input_update(void);
const InputState* Input_getState(void);

#endif // _INPUT_H_
