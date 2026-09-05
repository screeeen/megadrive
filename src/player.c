#include "player.h"
#include "resources.h"

#define PLAYER_MIN_X 0
#define PLAYER_MAX_X (320 - PLAYER_SPRITE_W)
#define PLAYER_MIN_Y 0
#define PLAYER_MAX_Y (224 - PLAYER_SPRITE_H)

static void updateHitbox(Player* player)
{
    player->hitboxX = player->x + ((PLAYER_SPRITE_W - PLAYER_HITBOX_SIZE) / 2);
    player->hitboxY = player->y + ((PLAYER_SPRITE_H - PLAYER_HITBOX_SIZE) / 2);
}

void Player_init(Player* player, s16 x, s16 y)
{
    player->x = x;
    player->y = y;
    player->velocityX = 0;
    player->velocityY = 0;
    player->direction = 0;
    player->speed = PLAYER_MAX_SPEED;

    player->weapon = WEAPON_VULCAN;
    player->weaponLevel = 1;
    player->lives = PLAYER_INITIAL_LIVES;
    player->bombs = PLAYER_INITIAL_BOMBS;

    player->state = PLAYER_STATE_NORMAL;
    player->invulnFrames = 0;
    player->speedBoostFrames = 0;

    updateHitbox(player);

    PAL_setPalette(PAL1, nx01.palette->data, DMA);
    player->sprite = SPR_addSprite(&nx01, x, y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
    SPR_setVisibility(player->sprite, VISIBLE);
}

void Player_respawn(Player* player, s16 x, s16 y)
{
    player->x = x;
    player->y = y;
    player->velocityX = 0;
    player->velocityY = 0;
    player->direction = 0;

    player->weapon = WEAPON_VULCAN;
    player->weaponLevel = 1;
    player->speed = PLAYER_MAX_SPEED;
    player->speedBoostFrames = 0;

    player->state = PLAYER_STATE_INVULNERABLE;
    player->invulnFrames = PLAYER_INVULN_FRAMES;

    updateHitbox(player);
    SPR_setPosition(player->sprite, player->x, player->y);
    SPR_setVisibility(player->sprite, VISIBLE);
}

void Player_hit(Player* player)
{
    if (player->state != PLAYER_STATE_NORMAL)
        return;

    PlayerHitState in;
    in.weaponLevel = player->weaponLevel;
    in.lives = player->lives;

    PlayerHitResult out = Player_applyHit(in);

    player->weaponLevel = out.state.weaponLevel;
    player->lives = out.state.lives;

    if (out.lifeLost)
    {
        // Caller (game_state.c) reads Player_isDead() and drives the
        // PLAYER_HIT/PLAYER_DEAD/GAME_OVER transitions.
        player->state = PLAYER_STATE_DEAD;
        SPR_setVisibility(player->sprite, HIDDEN);
    }
    else
    {
        player->state = PLAYER_STATE_INVULNERABLE;
        player->invulnFrames = PLAYER_INVULN_FRAMES;
    }
}

bool Player_isDead(const Player* player)
{
    return player->state == PLAYER_STATE_DEAD;
}

void Player_switchWeapon(Player* player)
{
    WeaponState in = { player->weapon, player->weaponLevel };
    WeaponState out = Weapon_switchNext(in);

    player->weapon = out.weapon;
    player->weaponLevel = out.weaponLevel;
}

void Player_levelUpWeapon(Player* player)
{
    WeaponState in = { player->weapon, player->weaponLevel };
    WeaponState out = Weapon_levelUp(in);

    player->weaponLevel = out.weaponLevel;
}

void Player_pickupWeapon(Player* player, WeaponType picked)
{
    WeaponState in = { player->weapon, player->weaponLevel };
    WeaponState out = Weapon_pickup(in, picked);

    player->weapon = out.weapon;
    player->weaponLevel = out.weaponLevel;
}

void Player_addBomb(Player* player)
{
    if (player->bombs < PLAYER_MAX_BOMBS)
        player->bombs++;
}

bool Player_useBomb(Player* player)
{
    if (player->bombs == 0)
        return FALSE;

    player->bombs--;
    return TRUE;
}

void Player_applySpeedBoost(Player* player)
{
    player->speed = PLAYER_MAX_SPEED + PLAYER_SPEED_BOOST_AMOUNT;
    player->speedBoostFrames = PLAYER_SPEED_BOOST_FRAMES;
}

void Player_addLife(Player* player)
{
    player->lives++;
}

void Player_update(Player* player, const InputState* input)
{
    if (player->state == PLAYER_STATE_DEAD)
        return;

    if (player->speedBoostFrames > 0)
    {
        if (--player->speedBoostFrames == 0)
            player->speed = PLAYER_MAX_SPEED;
    }

    u8 direction = 0;
    s16 vx = 0;
    s16 vy = 0;

    if (input->left)  { vx -= player->speed; direction |= BUTTON_LEFT; }
    if (input->right) { vx += player->speed; direction |= BUTTON_RIGHT; }
    if (input->up)    { vy -= player->speed; direction |= BUTTON_UP; }
    if (input->down)  { vy += player->speed; direction |= BUTTON_DOWN; }

    player->velocityX = vx;
    player->velocityY = vy;
    player->direction = direction;

    player->x += vx;
    player->y += vy;

    if (player->x < PLAYER_MIN_X) player->x = PLAYER_MIN_X;
    if (player->x > PLAYER_MAX_X) player->x = PLAYER_MAX_X;
    if (player->y < PLAYER_MIN_Y) player->y = PLAYER_MIN_Y;
    if (player->y > PLAYER_MAX_Y) player->y = PLAYER_MAX_Y;

    updateHitbox(player);

    if (player->state == PLAYER_STATE_INVULNERABLE)
    {
        player->invulnFrames--;

        bool blinkVisible = ((player->invulnFrames / PLAYER_BLINK_PERIOD) & 1) == 0;
        SPR_setVisibility(player->sprite, blinkVisible ? VISIBLE : HIDDEN);

        if (player->invulnFrames == 0)
        {
            player->state = PLAYER_STATE_NORMAL;
            SPR_setVisibility(player->sprite, VISIBLE);
        }
    }

    SPR_setPosition(player->sprite, player->x, player->y);
}
