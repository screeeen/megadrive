#include <genesis.h>
#include "game_state.h"
#include "input.h"
#include "player.h"
#include "audio.h"
#include "weapon.h"
#include "projectile.h"
#include "debug.h"

#define PLAYER_HIT_FRAMES 60 // ~1s beat before the respawn/game-over decision

static GameState state;
static Player player;
static u16 hitTimer;

static void Title_enter(void)
{
    VDP_clearPlane(BG_A, TRUE);
    VDP_drawText("NEON STRIKE", 10, 10);
    VDP_drawText("PRESS START", 10, 14);
}

static void Game_enter(void)
{
    VDP_clearPlane(BG_A, TRUE);
    VDP_drawText("HELLO WORLD!", 10, 12);

    Audio_init();
    Player_init(&player, 144, 96);
    Weapon_init();
    Projectile_poolInit();
}

static void Pause_enter(void)
{
    VDP_drawText("PAUSE", 14, 18);
}

static void Pause_exit(void)
{
    VDP_clearText(14, 18, 5);
}

static void PlayerHit_enter(void)
{
    hitTimer = PLAYER_HIT_FRAMES;
    VDP_drawText("HIT!", 15, 12);
}

static void GameOver_enter(void)
{
    // The player entity is fully rebuilt (fresh sprites) on the next
    // TITLE -> GAME transition; release what's on screen now so repeated
    // deaths/restarts don't leak hardware sprites.
    SPR_releaseSprite(player.sprite);
    if (player.debugHitboxSprite)
        SPR_releaseSprite(player.debugHitboxSprite);
    Projectile_releaseAll();

    VDP_clearPlane(BG_A, TRUE);
    VDP_drawText("GAME OVER", 12, 10);
    VDP_drawText("PRESS START", 10, 14);
}

#if SHOW_DEBUG_HUD
static void drawDebugHud(void)
{
    char buf[20];
    sprintf(buf, "LIVES:%d LV:%d", player.lives, player.weaponLevel);
    VDP_drawText(buf, 1, 26);
}
#endif

void GameState_init(void)
{
    state = STATE_TITLE;
    Title_enter();
}

void GameState_update(void)
{
    Input_update();
    const InputState* input = Input_getState();

#if SHOW_DEBUG_HUD
    VDP_showFPS(FALSE, 34, 0);
#endif

    GameState previous = state;
    GameState next = GameState_computeNext(previous, input->startPressed);

    if (next != previous)
    {
        state = next;

        if (previous == STATE_TITLE && next == STATE_GAME)
            Game_enter();
        else if (previous == STATE_GAME && next == STATE_PAUSE)
            Pause_enter();
        else if (previous == STATE_PAUSE && next == STATE_GAME)
            Pause_exit();
        else if (previous == STATE_GAME_OVER && next == STATE_TITLE)
            Title_enter();
    }

    switch (state)
    {
        case STATE_GAME:
            Player_update(&player, input);

            // Debug-only: no real hazard exists before M03 (weapons) / M05
            // (enemies) can hurt the player, so B is wired to Player_hit()
            // to exercise and visually validate damage/invulnerability/
            // death right now. Remove once real collisions land.
            if (input->bPressed)
                Player_hit(&player);

            if (Player_isDead(&player))
            {
                state = STATE_PLAYER_HIT;
                PlayerHit_enter();
                break;
            }

            Weapon_update(&player, input);
            Projectile_poolUpdate();

            Audio_update();
            SPR_update();

#if SHOW_DEBUG_HUD
            drawDebugHud();
#endif
            break;

        case STATE_PLAYER_HIT:
            Audio_update();

            if (--hitTimer == 0)
            {
                VDP_clearText(15, 12, 4);

                if (player.lives > 0)
                {
                    state = STATE_GAME;
                    Player_respawn(&player, 144, 96);
                }
                else
                {
                    state = STATE_GAME_OVER;
                    GameOver_enter();
                }
            }
            break;

        case STATE_PAUSE:
            // Gameplay and sprite animation freeze; music keeps playing
            // (arcade convention) so pause never looks like a hang.
            Audio_update();
            break;

        case STATE_TITLE:
        case STATE_GAME_OVER:
        default:
            break;
    }
}
