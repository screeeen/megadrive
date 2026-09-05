#include <genesis.h>
#include "game_state.h"
#include "input.h"
#include "player.h"
#include "audio.h"
#include "weapon.h"
#include "projectile.h"
#include "enemy.h"
#include "powerup.h"
#include "combat.h"
#include "combo.h"
#include "bomb.h"
#include "score.h"
#include "debug.h"

#define PLAYER_HIT_FRAMES 60 // ~1s beat before the respawn/game-over decision
#define CONTINUE_FRAMES   90 // ~1.5s "CONTINUE?" beat before gameplay resumes

// SPEC.md §26: 3 initial continues per campaign.
#define INITIAL_CONTINUES 3

#define BOMB_FLASH_FRAMES 20 // how long the "BOMB!" feedback text stays up

// No Spawn Manager/stage data exists yet (M07); this cycles through all 7
// enemy types on a timer purely so M05/M06 have something to validate
// against. Replaced outright once M07 lands real stage spawn data.
#define DEBUG_SPAWN_INTERVAL 90

static GameState state;
static Player player;
static u16 hitTimer;
static u16 continueTimer;
static u16 bombFlashTimer;
static u8 continuesRemaining;
static u16 debugSpawnTimer;
static EnemyType debugNextEnemyType;

static void Title_enter(void)
{
    VDP_clearPlane(BG_A, TRUE);
    VDP_drawText("NEON STRIKE", 10, 10);
    VDP_drawText("PRESS START", 10, 14);
}

// Shared by a fresh game (Game_enter) and resuming after a continue
// (Continue_resume): every subsystem pool + the debug spawner.
static void initSystems(void)
{
    Weapon_init();
    Projectile_poolInit();
    Enemy_poolInit();
    Powerup_poolInit();
    Combo_reset();

    debugSpawnTimer = DEBUG_SPAWN_INTERVAL;
    debugNextEnemyType = ENEMY_DRONE;
}

static void Game_enter(void)
{
    VDP_clearPlane(BG_A, TRUE);

    Audio_init();
    Player_init(&player, 144, 96);
    initSystems();
    Score_reset();

    continuesRemaining = INITIAL_CONTINUES;
}

// Debug-only stand-in for M07's real Stage Data System: spawns the next
// enemy type in sequence, off the right edge, at a Y that avoids exact
// stacking. Not a real formation/spawn-timeline — just enough to exercise
// every enemy type and combat.c's collisions during M05/M06.
static void debugSpawnEnemies(void)
{
    if (--debugSpawnTimer > 0)
        return;

    debugSpawnTimer = DEBUG_SPAWN_INTERVAL;

    s16 y = 20 + (debugNextEnemyType * 25);
    Enemy_spawn(debugNextEnemyType, 300, y);

    debugNextEnemyType = (EnemyType) ((debugNextEnemyType + 1) % ENEMY_TYPE_COUNT);
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
    // TITLE -> GAME (or CONTINUE -> GAME) transition; release what's on
    // screen now so repeated deaths/restarts don't leak hardware sprites.
    SPR_releaseSprite(player.sprite);
    Projectile_releaseAll();
    Enemy_releaseAll();
    Powerup_releaseAll();

    // The last note Audio_update() set otherwise keeps sounding forever —
    // PSG channels latch, they don't stop on their own.
    Audio_stop();

    VDP_clearPlane(BG_A, TRUE);
    VDP_drawText("GAME OVER", 12, 10);

    char buf[16];
    sprintf(buf, "SCORE:%lu", Score_get());
    VDP_drawText(buf, 13, 12);

    if (continuesRemaining > 0)
        VDP_drawText("START: CONTINUE", 8, 15);
    else
        VDP_drawText("PRESS START", 10, 15);
}

static void Continue_enter(void)
{
    continueTimer = CONTINUE_FRAMES;

    VDP_clearPlane(BG_A, TRUE);
    VDP_drawText("CONTINUE?", 12, 10);

    char buf[16];
    sprintf(buf, "%d LEFT", continuesRemaining);
    VDP_drawText(buf, 14, 12);
}

static void Continue_resume(void)
{
    VDP_clearPlane(BG_A, TRUE);

    Audio_init();
    Player_init(&player, 144, 96);
    player.bombs = 1; // SPEC.md §26: "se mantiene una bomba"
    initSystems();
    // Score is deliberately NOT reset — kept across a continue.
}

#if SHOW_DEBUG_HUD
static const char* weaponName(WeaponType weapon)
{
    switch (weapon)
    {
        case WEAPON_VULCAN: return "VULCAN";
        case WEAPON_LASER:  return "LASER";
        case WEAPON_WIDE:   return "WIDE";
        case WEAPON_HOMING: return "HOMING";
        case WEAPON_FLAME:  return "FLAME";
        default:            return "?";
    }
}

static void drawDebugHud(void)
{
    char buf[40];
    sprintf(buf, "LIVES:%d %s LV:%d BOMB:%d x%d SCORE:%lu", player.lives,
            weaponName(player.weapon), player.weaponLevel, player.bombs,
            Combo_getMultiplier(), Score_get());
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
    }

    switch (state)
    {
        case STATE_GAME:
            Player_update(&player, input);

            // B (SPEC.md §5): weapon switching.
            if (input->bPressed)
                Player_switchWeapon(&player);

            // C (SPEC.md §5): bomb. Resolves the M03/M06 double-booking —
            // this button's only debug job (M03's weapon-level-up trigger)
            // is retired now that real weapon/P power-ups exist to reach
            // L2/L3 in-game.
            if (input->cPressed && Bomb_use(&player))
                bombFlashTimer = BOMB_FLASH_FRAMES;

            Weapon_update(&player, input);
            Projectile_poolUpdate();
            Enemy_poolUpdate(player.x, player.y);
            Powerup_poolUpdate();
            Combo_update();
            Combat_resolveCollisions(&player);
            debugSpawnEnemies();

            if (Player_isDead(&player))
            {
                state = STATE_PLAYER_HIT;
                PlayerHit_enter();
                break;
            }

            Audio_update();
            SPR_update();

            if (bombFlashTimer > 0)
            {
                if (--bombFlashTimer == 0)
                    VDP_clearText(16, 4, 5);
                else if (bombFlashTimer == BOMB_FLASH_FRAMES - 1)
                    VDP_drawText("BOMB!", 16, 4);
            }

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

        case STATE_GAME_OVER:
            if (input->startPressed)
            {
                if (continuesRemaining > 0)
                {
                    continuesRemaining--;
                    state = STATE_CONTINUE;
                    Continue_enter();
                }
                else
                {
                    state = STATE_TITLE;
                    Title_enter();
                }
            }
            break;

        case STATE_CONTINUE:
            if (--continueTimer == 0)
            {
                state = STATE_GAME;
                Continue_resume();
            }
            break;

        case STATE_PAUSE:
            // Gameplay and sprite animation freeze; music keeps playing
            // (arcade convention) so pause never looks like a hang.
            Audio_update();
            break;

        case STATE_TITLE:
        default:
            break;
    }
}
