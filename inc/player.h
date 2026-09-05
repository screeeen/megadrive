#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <genesis.h>
#include "input.h"

typedef struct
{
    s16 x;
    s16 y;
    Sprite* sprite;
} Player;

void Player_init(Player* player, s16 x, s16 y);
void Player_update(Player* player, const InputState* input);

#endif // _PLAYER_H_
