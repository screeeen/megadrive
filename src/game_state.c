#include <genesis.h>
#include "game_state.h"
#include "input.h"
#include "player.h"
#include "audio.h"
#include "weapon.h"
#include "projectile.h"
#include "enemy.h"
#include "powerup.h"
#include "boss.h"
#include "combat.h"
#include "combo.h"
#include "bomb.h"
#include "score.h"
#include "stage.h"
#include "spawn_manager.h"
#include "scroll.h"
#include "debug.h"

#define PLAYER_HIT_FRAMES   60 // ~1s beat before the respawn/game-over decision
#define CONTINUE_FRAMES     90 // ~1.5s "CONTINUE?" beat before gameplay resumes
#define STAGE_CLEAR_FRAMES  90
#define NEXT_STAGE_FRAMES   60

// SPEC.md §26: 3 initial continues per campaign.
#define INITIAL_CONTINUES 3

#define BOMB_FLASH_FRAMES 20 // how long the "BOMB!" feedback text stays up

// SPEC.md §22 Orbital Guardian: ~250 HP.
#define ORBITAL_GUARDIAN_MAX_HP 250
#define ORBITAL_GUARDIAN_SCORE  50000 // SPEC.md §23: boss = 50,000
#define BOSS_SPAWN_X 340
#define BOSS_SPAWN_Y 80

// M08's real content. M07's `testStage` remains available (unused here) as
// a fast, generic-system regression check — see PROGRESS.md.
static const StageDef* currentStage = &stage1;

static GameState state;
static Player player;
static Boss boss;
static u16 hitTimer;
static u16 continueTimer;
static u16 stageClearTimer;
static u16 nextStageTimer;
static u16 bombFlashTimer;
static u8 continuesRemaining;
static s16 lastDrawnBossHp = -1; // -1 forces a redraw the first frame a boss is on screen

// No boss HP feedback existed at all (not even for debugging) — with no way
// to see the number go down, a "the boss never dies" report is impossible
// to tell apart from a real bug. Simple text readout for now; a real
// graphical bar is a later-milestone polish item.
static void drawBossHp(s16 hp, s16 maxHp)
{
    if (hp == lastDrawnBossHp)
        return;

    lastDrawnBossHp = hp;

    char buf[20];
    sprintf(buf, "BOSS HP:%3d/%3d", hp, maxHp);
    VDP_clearText(1, 1, 16);
    VDP_drawText(buf, 1, 1);
}

static void clearBossHp(void)
{
    lastDrawnBossHp = -1;
    VDP_clearText(1, 1, 16);
}

static void Title_enter(void)
{
    VDP_clearPlane(BG_A, TRUE);
    VDP_drawText("NEON STRIKE", 10, 10);
    VDP_drawText("PRESS START", 10, 14);
}

// Clears every gameplay pool and starts (or resumes, from a checkpoint)
// the stage timeline + scroll. Shared by every path that puts a Player on
// screen with a clean slate: a fresh game, a continue, a mid-stage
// respawn, and looping into the next stage attempt.
static void resetGameplayPools(void)
{
    Weapon_init();

    // Release before re-init: on respawn/next-stage there can still be
    // active enemies/projectiles/powerups on screen (only the player died,
    // or the stage timeline just ended — other entities weren't touched).
    // The *_poolInit() functions only clear bookkeeping (active flags),
    // they never call SPR_releaseSprite(); skipping the release step here
    // orphaned their sprites in SGDK's sprite engine, which kept rendering
    // them forever ("enemies stay painted on screen" after a hit).
    Projectile_releaseAll();
    Projectile_poolInit();
    Enemy_releaseAll();
    Enemy_poolInit();
    Powerup_releaseAll();
    Powerup_poolInit();
    Boss_release(&boss);

    Combo_reset();
    Scroll_init(currentStage->backgroundId);
}

static void Game_enter(void)
{
    VDP_clearPlane(BG_A, TRUE);

    Audio_init();
    Player_init(&player, 144, 96);
    resetGameplayPools();
    SpawnManager_start(currentStage);
    Score_reset();

    continuesRemaining = INITIAL_CONTINUES;
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

// SPEC.md §20: dying rewinds the stage timeline to the last checkpoint
// passed (0 = stage start) rather than continuing exactly where the
// player died — content between the checkpoint and the death point plays
// again, the standard shmup convention.
static void respawnAtCheckpoint(void)
{
    u8 checkpoints = SpawnManager_getCheckpointsPassed();

    Player_respawn(&player, 144, 96);
    resetGameplayPools();
    SpawnManager_resumeFromCheckpoint(currentStage, checkpoints);
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
    Boss_release(&boss);

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
    resetGameplayPools();
    SpawnManager_resumeFromCheckpoint(currentStage, SpawnManager_getCheckpointsPassed());
    // Score is deliberately NOT reset — kept across a continue.
}

static void StageClear_enter(void)
{
    stageClearTimer = STAGE_CLEAR_FRAMES;

    // Same fix as GameOver_enter(): STATE_STAGE_CLEAR never calls
    // Audio_update(), so whatever note was sounding when gameplay ended
    // otherwise keeps sounding forever (PSG channels latch, they don't stop
    // on their own).
    Audio_stop();

    VDP_drawText("STAGE CLEAR", 10, 10);
}

static void NextStage_enter(void)
{
    nextStageTimer = NEXT_STAGE_FRAMES;
    VDP_clearText(10, 10, 11);
    VDP_drawText("NEXT STAGE", 10, 10);
}

static void NextStage_resume(void)
{
    // Loops back into the same Stage 1 — Stage 2+ content is M10+'s job.
    // Weapon/level/lives/score/bombs carry over; only the gameplay pools
    // and the timeline/scroll reset for the new attempt.
    VDP_clearPlane(BG_A, TRUE);

    // StageClear_enter()'s Audio_stop() silences the PSG channel's envelope
    // and nothing re-enables it — without this, gameplay would resume
    // permanently muted (same "PSG channels latch" issue as the GAME_OVER
    // and STAGE_CLEAR beep fixes, just missing on the "make sound come
    // back" side instead of "make it stop").
    Audio_init();
    Player_respawn(&player, 144, 96);
    resetGameplayPools();
    SpawnManager_start(currentStage);
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

            // C (SPEC.md §5): bomb.
            if (input->cPressed && Bomb_use(&player))
                bombFlashTimer = BOMB_FLASH_FRAMES;

            Weapon_update(&player, input);
            Projectile_poolUpdate();
            Enemy_poolUpdate(player.x, player.y);
            Powerup_poolUpdate();
            Combo_update();
            Combat_resolveCollisions(&player, &boss);
            SpawnManager_update();
            Scroll_update(currentStage->scrollSpeed);

            if (Player_isDead(&player))
            {
                state = STATE_PLAYER_HIT;
                PlayerHit_enter();
                break;
            }

            if (SpawnManager_isComplete())
            {
                state = STATE_BOSS;
                Boss_spawn(&boss, BOSS_SPAWN_X, BOSS_SPAWN_Y, ORBITAL_GUARDIAN_MAX_HP, ORBITAL_GUARDIAN_SCORE);
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

        case STATE_BOSS:
            Player_update(&player, input);

            if (input->bPressed)
                Player_switchWeapon(&player);

            if (input->cPressed && Bomb_use(&player))
                bombFlashTimer = BOMB_FLASH_FRAMES;

            Weapon_update(&player, input);
            Projectile_poolUpdate();
            Enemy_poolUpdate(player.x, player.y); // phase 2's drone spawns need this to move/fire
            Boss_update(&boss, player.x, player.y);
            Combat_resolveCollisions(&player, &boss);
            drawBossHp(boss.hp, boss.maxHp);

            if (Player_isDead(&player))
            {
                state = STATE_PLAYER_HIT;
                PlayerHit_enter();
                clearBossHp();
                break;
            }

            if (Boss_isEncounterOver(&boss))
            {
                Score_add((u16) (boss.score * Combo_getMultiplier()));
                Combo_onKill();
                Boss_release(&boss);
                clearBossHp();
                state = STATE_STAGE_CLEAR;
                StageClear_enter();
                break;
            }

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
                    respawnAtCheckpoint();
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

        case STATE_STAGE_CLEAR:
            if (--stageClearTimer == 0)
            {
                state = STATE_NEXT_STAGE;
                NextStage_enter();
            }
            break;

        case STATE_NEXT_STAGE:
            if (--nextStageTimer == 0)
            {
                state = STATE_GAME;
                NextStage_resume();
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
