#include "player.h"
#include "resources.h"

#define PLAYER_SPEED   2

#define PLAYER_MIN_X   0
#define PLAYER_MAX_X   (320 - 32)
#define PLAYER_MIN_Y   0
#define PLAYER_MAX_Y   (224 - 32)

void Player_init(Player* player, s16 x, s16 y)
{
    player->x = x;
    player->y = y;

    PAL_setPalette(PAL1, donut.palette->data, DMA);
    player->sprite = SPR_addSprite(&donut, x, y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
}

void Player_update(Player* player, const InputState* input)
{
    if (input->left)  player->x -= PLAYER_SPEED;
    if (input->right) player->x += PLAYER_SPEED;
    if (input->up)    player->y -= PLAYER_SPEED;
    if (input->down)  player->y += PLAYER_SPEED;

    if (player->x < PLAYER_MIN_X) player->x = PLAYER_MIN_X;
    if (player->x > PLAYER_MAX_X) player->x = PLAYER_MAX_X;
    if (player->y < PLAYER_MIN_Y) player->y = PLAYER_MIN_Y;
    if (player->y > PLAYER_MAX_Y) player->y = PLAYER_MAX_Y;

    SPR_setPosition(player->sprite, player->x, player->y);
}
