#ifndef _INPUT_H_
#define _INPUT_H_

#include <genesis.h>

typedef struct
{
    bool up;
    bool down;
    bool left;
    bool right;
    bool start;         // held
    bool startPressed;  // true only the frame START goes from released to held
    bool a;             // held (fires the current weapon)
    bool b;             // held (reserved: M04 weapon switch)
    bool bPressed;      // true only the frame B goes from released to held
    bool cPressed;      // true only the frame C goes from released to held
} InputState;

void Input_init(void);
void Input_update(void);
const InputState* Input_getState(void);

#endif // _INPUT_H_
