#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <genesis.h>
#include "input.h"
#include "player_logic.h"
#include "debug.h"

// SPEC.md §6 (Player)
#define PLAYER_SPRITE_W          24
#define PLAYER_SPRITE_H          16
#define PLAYER_HITBOX_SIZE       6
#define PLAYER_MAX_SPEED         5   // px/frame
#define PLAYER_INITIAL_LIVES     3
#define PLAYER_INITIAL_BOMBS     3
#define PLAYER_WEAPON_LEVEL_MAX  3
#define PLAYER_INVULN_FRAMES     90  // ~1.5s at 60Hz
#define PLAYER_BLINK_PERIOD      8   // frames per visibility toggle while invulnerable

typedef enum
{
    PLAYER_STATE_NORMAL,
    PLAYER_STATE_INVULNERABLE,
    PLAYER_STATE_DEAD
} PlayerState;

typedef struct
{
    s16 x;
    s16 y;
    s16 velocityX;
    s16 velocityY;
    u8  direction; // bitmask: BUTTON_UP/DOWN/LEFT/RIGHT currently held
    u8  speed;     // current max px/frame (power-ups may raise this later)

    s16 hitboxX; // top-left of the 6x6 hitbox, derived from x/y each frame
    s16 hitboxY;

    WeaponType weapon;
    u8 weaponLevel;
    u8 lives;
    u8 bombs;

    PlayerState state;
    u16 invulnFrames;

    Sprite* sprite;
    Sprite* debugHitboxSprite;
} Player;

void Player_init(Player* player, s16 x, s16 y);
void Player_update(Player* player, const InputState* input);

// Applies one hit (see player_logic.h for the resolution rules). No-op if
// the player is already invulnerable or dead.
void Player_hit(Player* player);

// Resets position/weapon/invulnerability for a new life after a death that
// didn't end the game. Lives/bombs/score are left untouched.
void Player_respawn(Player* player, s16 x, s16 y);

bool Player_isDead(const Player* player);

// B button (SPEC.md §5): advances to the next weapon type, reset to L1.
void Player_switchWeapon(Player* player);

// Same-weapon pickup (SPEC.md §8): +1 level, capped at 3. No power-up
// system exists yet (M06); exposed for the M04 debug level-up trigger.
void Player_levelUpWeapon(Player* player);

#endif // _PLAYER_H_
