#include <genesis.h>
#include "game_state.h"
#include "input.h"
#include "player.h"
#include "audio.h"
#include "weapon.h"
#include "projectile.h"
#include "enemy.h"
#include "powerup.h"
#include "asteroid.h"
#include "explosion.h"
#include "boss.h"
#include "boss_data.h"
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

// Where the boss's INTRO approach starts from — the boss's own HP/score/
// vulnerable points/attacks are BossDef data now (src/boss_data.c), not
// game_state.c's job to know.
#define BOSS_SPAWN_X 340
#define BOSS_SPAWN_Y 80

// boss_data.c's heliosDef is BossDef_forId(6) — the one stage boss whose
// defeat ends the campaign (SPEC.md §22) instead of looping into the next
// stage. Checked by bossId rather than by stage index so this stays
// correct even if the campaign array's order ever changes.
#define FINAL_BOSS_ID 6
#define ENDING_FRAMES 180 // ~3s explosion/"scroll stopped" beat before credits

// M08/M10/M11/M12's real content, played in order. M07's `testStage`
// remains available (unused here) as a fast, generic-system regression
// check — see PROGRESS.md. The real Final Boss experience (M13: Helios's
// death sequence, ENDING/CREDITS transition, ranking) isn't wired up yet,
// so clearing Stage 6 currently just loops back to Stage 1 like every
// earlier stage did before the next one existed.
static const StageDef* const campaign[] = { &stage1, &stage2, &stage3, &stage4, &stage5, &stage6 };
#define CAMPAIGN_LENGTH (sizeof(campaign) / sizeof(campaign[0]))
static u8 stageIndex;
#define currentStage (campaign[stageIndex])

// Display names, indexed the same as `campaign` — user-requested HUD
// readout (there was previously no way to confirm which stage was
// actually active, which is exactly what made M11's "no veo las rocas"
// report hard to pin down: was Stage 4 never reached, or reached but not
// rendering asteroids?).
static const char* const stageNames[] = {
    "ORBITAL CITY", "RED DESERT", "SPACE COLONY", "ASTEROID BELT", "HELIOS FLEET", "HELIOS CORE"
};

static GameState state;
static Player player;
static Boss boss;
static u16 hitTimer;
static u16 continueTimer;
static u16 stageClearTimer;
static u16 nextStageTimer;
static u16 endingTimer;
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
    Asteroid_releaseAll();
    Asteroid_poolInit();
    Explosion_releaseAll();
    Explosion_poolInit();
    Boss_release(&boss);

    Combo_reset();
    Scroll_init(currentStage->backgroundId);
}

static void Game_enter(void)
{
    VDP_clearPlane(BG_A, TRUE);

    stageIndex = 0; // a fresh game always starts the campaign at Stage 1
    Audio_init();
    Audio_playMusic(currentStage->musicId);
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
    Explosion_spawn(player.x, player.y); // only reached on a life-costing hit (Player_isDead), not every hit
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
    Asteroid_releaseAll();
    Explosion_releaseAll();
    Boss_release(&boss);

    // The last note Audio_update() set otherwise keeps sounding forever —
    // PSG channels latch, they don't stop on their own.
    Audio_stop();

    VDP_clearPlane(BG_A, TRUE);
    VDP_drawText("GAME OVER", 12, 10);

    // "SCORE:" (6 chars) + up to 10 digits (u32 max) + null = 17 bytes
    // worst case — 16 was 1 byte short of that; found while auditing every
    // sprintf buffer after drawDebugHud's real overflow (see its comment).
    char buf[20];
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
    Audio_playMusic(currentStage->musicId);
    Player_init(&player, 144, 96);
    player.bombs = 1; // SPEC.md §26: "se mantiene una bomba"
    resetGameplayPools();
    SpawnManager_resumeFromCheckpoint(currentStage, SpawnManager_getCheckpointsPassed());
    // Score is deliberately NOT reset — kept across a continue.
}

// MILESTONES.md M14: "stage clear" wants a score bonus, but SPEC.md never
// gives an exact value anywhere (checked GAME_SPEC.md for "bonus" —
// nothing) — chosen here, same rationale as every other not-specified
// number in this project.
#define STAGE_CLEAR_BONUS 1000

static void StageClear_enter(void)
{
    stageClearTimer = STAGE_CLEAR_FRAMES;

    // Same fix as GameOver_enter(): STATE_STAGE_CLEAR never calls
    // Audio_update(), so whatever note was sounding when gameplay ended
    // otherwise keeps sounding forever (PSG channels latch, they don't stop
    // on their own).
    Audio_stop();
    Audio_playSfx(SFX_STAGE_CLEAR); // after Audio_stop(), which also silences the SFX channel

    Score_add(STAGE_CLEAR_BONUS);

    VDP_drawText("STAGE CLEAR", 10, 10);

    char buf[16];
    sprintf(buf, "BONUS:%d", STAGE_CLEAR_BONUS);
    VDP_drawText(buf, 13, 12);
}

static void NextStage_enter(void)
{
    nextStageTimer = NEXT_STAGE_FRAMES;
    VDP_clearText(10, 10, 11);
    VDP_drawText("NEXT STAGE", 10, 10);
}

static void NextStage_resume(void)
{
    // Advances to the next stage in the campaign, looping back to Stage 1
    // once Stage 6 is cleared — the real Final Boss ending (M13) isn't
    // wired up yet, see this file's `campaign` comment. Weapon/level/
    // lives/score/bombs carry over; only the gameplay pools and the
    // timeline/scroll reset for the new stage.
    stageIndex = (u8) ((stageIndex + 1) % CAMPAIGN_LENGTH);

    VDP_clearPlane(BG_A, TRUE);

    // StageClear_enter()'s Audio_stop() silences the PSG channel's envelope
    // and nothing re-enables it — without this, gameplay would resume
    // permanently muted (same "PSG channels latch" issue as the GAME_OVER
    // and STAGE_CLEAR beep fixes, just missing on the "make sound come
    // back" side instead of "make it stop").
    Audio_init();
    Audio_playMusic(currentStage->musicId); // the new stage's own theme (stageIndex already advanced above)
    Player_respawn(&player, 144, 96);
    resetGameplayPools();
    SpawnManager_start(currentStage);
}

// SPEC.md §22: "Al destruir HELIOS: Explosion. Scroll detenido. Secuencia
// final. Creditos. Ranking." — the real Final Boss ending (M13), distinct
// from every other boss's STAGE_CLEAR/NEXT_STAGE loop. "Scroll detenido"
// falls out naturally: STATE_ENDING/STATE_CREDITS never call
// Scroll_update(). No dedicated explosion sprite/particle system exists
// (same placeholder-era gap as every other entity's death in this engine,
// M16's job) — the boss's own DEATH-state strobe (boss.c) is the visual
// beat; this text is the "secuencia final" on top of it.
static void Ending_enter(void)
{
    endingTimer = ENDING_FRAMES;

    // Same cleanup as GameOver_enter(): nothing else releases these once
    // gameplay stops for good, and leaving them would clutter the ending
    // screen with stray sprites (the boss itself is already released by
    // the caller before this runs).
    SPR_releaseSprite(player.sprite);
    Projectile_releaseAll();
    Enemy_releaseAll();
    Powerup_releaseAll();
    Asteroid_releaseAll();
    Explosion_releaseAll();

    Audio_stop();
    VDP_clearPlane(BG_A, TRUE);
    VDP_drawText("HELIOS DESTROYED", 8, 10);
    VDP_drawText("ORBIT SECURED", 9, 12);
}

// "Ranking" (SPEC.md §22) would need persistent high-score storage (SRAM/
// EEPROM) — no save system of any kind exists anywhere in this project,
// so this shows the run's final score instead of a real leaderboard,
// documented here rather than silently treated as the same thing.
static void Credits_enter(void)
{
    Audio_playMusic(MUSIC_CREDITS);

    VDP_clearPlane(BG_A, TRUE);
    VDP_drawText("NEON STRIKE", 10, 8);
    VDP_drawText("THE END", 12, 11);

    char buf[24];
    sprintf(buf, "FINAL SCORE:%lu", Score_get());
    VDP_drawText(buf, 9, 14);

    VDP_drawText("PRESS START", 10, 18);
}

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

// SPEC.md §22/M14: the real, always-on HUD (previously only a
// SHOW_DEBUG_HUD-gated line, per AGENT.md §15 not meant to ship in a real
// build). Top: SCORE at the left, HI at the right corner (user-requested
// layout — originally both sat together on the left per SPEC.md's literal
// "SCORE 00124500 HI 9999999" example). Bottom: LIVES, weapon+level (with
// a "POWER" bar of #'s, one per weapon level, standing in for a real
// graphical bar — no bar-drawing tile exists), BOMB count, combo
// multiplier. Placed at rows 0/26 to stay clear of the boss HP/stage-
// progress (row 1), VULNERABLE!/HIT!/PAUSE text in the middle rows.
static void drawHud(void)
{
    static u32 lastScore = 0xFFFFFFFF; // force a redraw the first frame
    u32 currentScore = Score_get();

    if (currentScore != lastScore)
    {
        lastScore = currentScore;

        char buf[16];
        sprintf(buf, "SCORE:%08lu", currentScore);
        VDP_drawText(buf, 1, 0);

        // Right-aligned: "HI:00000000" is 11 chars, ending at column 38 on
        // the 40-column plane (columns 28-38), one column of margin from
        // the right edge.
        sprintf(buf, "HI:%08lu", Score_getHigh());
        VDP_drawText(buf, 28, 0);
    }

    // Real M17 perf bug found during stress testing: this line redrew
    // unconditionally every single frame (a VDP_clearText + VDP_drawText
    // of ~30-40 tiles, 60 times/second, regardless of whether anything
    // changed) — the SCORE/HI line above already guarded on value change,
    // this one never did. That fixed per-frame cost was enough on its own
    // to push some frames close to the 16.67ms budget; firing Wide (up to
    // 7 SPR_addSprite calls in one frame at L3) was just the extra push
    // that tipped individual frames over it, which is why the slowdown
    // tracked with "using Wide" even in sparse, realistic gameplay rather
    // than only in an intentionally maxed-out stress scene.
    static s8 lastLives = -1;
    static WeaponType lastWeapon = WEAPON_COUNT;
    static u8 lastWeaponLevel = 0;
    static s8 lastBombs = -1;
    static u8 lastCombo = 0;

    if (player.lives != lastLives || player.weapon != lastWeapon ||
        player.weaponLevel != lastWeaponLevel || player.bombs != lastBombs ||
        Combo_getMultiplier() != lastCombo)
    {
        lastLives = (s8) player.lives;
        lastWeapon = player.weapon;
        lastWeaponLevel = player.weaponLevel;
        lastBombs = (s8) player.bombs;
        lastCombo = Combo_getMultiplier();

        char powerBar[4] = { '#', '#', '#', 0 };
        powerBar[player.weaponLevel] = 0; // weaponLevel is 1-3, truncate to that many '#'

        // Kept deliberately short: the 40-column text plane only leaves 39
        // usable columns from x=1, and "LIVES:9 WEAPON:VULCAN POWER:###
        // BOMB:9 x5" would overflow that — weapon name is immediately
        // followed by its power-level '#'s instead of a separate "POWER:"
        // label so the whole line fits with room to spare ("LIVES:9
        // VULCAN### BOMB:9 x5" = 27 chars worst case). Buffer still sized
        // well past that (NS-12 discipline: size for the true worst case,
        // not the common one).
        char buf[40];
        sprintf(buf, "LIVES:%d %s%s BOMB:%d x%d", player.lives,
                weaponName(player.weapon), powerBar, player.bombs, Combo_getMultiplier());
        VDP_clearText(1, 26, 39); // 40-column text plane, column 1 to the right edge
        VDP_drawText(buf, 1, 26);
    }
}

// User-requested: how much of the current stage is left. Only meaningful
// during STATE_GAME (once STATE_BOSS starts, the timeline is complete —
// drawBossHp() takes over this same row/column area instead, and its
// wider VDP_clearText(1,1,16) already covers this text's shorter width,
// so no stale digits linger when the boss fight begins).
static void drawStageProgress(void)
{
    static u8 lastPercent = 0xFF; // force a redraw the first frame of every stage

    u32 frame = SpawnManager_getCurrentFrame();
    u32 length = currentStage->lengthFrames;
    u8 percent = (u8) (frame >= length ? 100 : (frame * 100 / length));

    if (percent == lastPercent)
        return;

    lastPercent = percent;

    char buf[12];
    sprintf(buf, "STAGE:%3d%%", percent);
    VDP_clearText(1, 1, 12);
    VDP_drawText(buf, 1, 1);
}

#if SHOW_DEBUG_HUD
static void drawDebugHud(void)
{
    // Stage name/index in the HUD (user-requested) plus the active
    // asteroid count — added while diagnosing "no veo las rocas" (M11), to
    // tell apart "Stage 4 never actually starts" from "asteroids spawn but
    // don't render" if that's ever reported again. Development-only, per
    // AGENT.md §15 — the real HUD is drawHud() above, unconditional.
    //
    // Same real M17 perf bug as drawHud()'s bottom line, found in the same
    // pass: this redrew unconditionally every frame, and since SHOW_DEBUG_
    // HUD is 1 throughout normal development testing, it was adding this
    // fixed per-frame cost to every single build tested this whole
    // project — now guarded on value change like everything else.
    static u8 lastStageIndex = 0xFF;
    static u8 lastActiveAsteroids = 0xFF;

    Asteroid* asteroids = Asteroid_getPool();
    u8 activeAsteroids = 0;
    for (u16 i = 0; i < ASTEROID_POOL_SIZE; i++)
        if (asteroids[i].active)
            activeAsteroids++;

    if (stageIndex == lastStageIndex && activeAsteroids == lastActiveAsteroids)
        return;

    lastStageIndex = stageIndex;
    lastActiveAsteroids = activeAsteroids;

    char stageBuf[40];
    sprintf(stageBuf, "STAGE %d/%d: %s AST:%d", stageIndex + 1, (int) CAMPAIGN_LENGTH,
            stageNames[stageIndex], activeAsteroids);
    VDP_clearText(1, 27, 39);
    VDP_drawText(stageBuf, 1, 27);
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
    VDP_showFPS(FALSE, 34, 1); // row 1: below the real HUD's HI score (row 0, right corner)
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
            Asteroid_poolUpdate();
            Explosion_poolUpdate();
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
                Boss_spawn(&boss, BossDef_forId(currentStage->bossId), BOSS_SPAWN_X, BOSS_SPAWN_Y);
                Audio_playMusic(currentStage->bossId == FINAL_BOSS_ID ? MUSIC_FINAL_BOSS : MUSIC_BOSS);
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

            drawHud();
            drawStageProgress();
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
            Asteroid_poolUpdate(); // any asteroid still drifting when the boss trigger fires
            Explosion_poolUpdate();
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
                bool finalBoss = currentStage->bossId == FINAL_BOSS_ID;

                Score_add((u16) (boss.score * Combo_getMultiplier()));
                Combo_onKill();
                Boss_release(&boss);
                clearBossHp();

                // In practice this gets cut short almost immediately by
                // whichever of StageClear_enter()'s SFX_STAGE_CLEAR or
                // Ending_enter()'s Audio_stop() runs right after it, since
                // both share the same single spare tone channel — an
                // honest, minor limitation of this placeholder-era SFX
                // system (see PROGRESS.md's M15 summary), not worth a
                // priority/queueing system for.
                Audio_playSfx(SFX_BOSS);

                // A bigger visual payoff than a single enemy's explosion:
                // several bursts spread across the boss's own footprint
                // (boss.x/y are still valid — Boss_release() above only
                // released the sprite, not the struct's position data).
                Explosion_spawn(boss.x, boss.y);
                Explosion_spawn((s16) (boss.x + BOSS_SPRITE_W / 2), (s16) (boss.y + BOSS_SPRITE_H / 2));
                Explosion_spawn((s16) (boss.x + BOSS_SPRITE_W - 8), (s16) (boss.y + BOSS_SPRITE_H - 8));

                if (finalBoss)
                {
                    state = STATE_ENDING;
                    Ending_enter();
                }
                else
                {
                    state = STATE_STAGE_CLEAR;
                    StageClear_enter();
                }
                break;
            }

            Audio_update();
            SPR_update();

            drawHud();
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
            // Audio_stop() already silenced channel 0 (the music); calling
            // Audio_update() here is safe (it only recomputes channel 0's
            // frequency, inaudible while its envelope is muted) and is
            // what lets the SFX_STAGE_CLEAR fanfare triggered in
            // StageClear_enter() actually play through its notes and
            // self-silence — without this call, that fanfare would freeze
            // on its first note for the rest of this state (a hidden
            // repeat of the exact "PSG channels latch" bug class fixed at
            // M06/M09, just on the SFX channel this time).
            Audio_update();

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

        case STATE_ENDING:
            if (--endingTimer == 0)
            {
                state = STATE_CREDITS;
                Credits_enter();
            }
            break;

        case STATE_CREDITS:
            Audio_update(); // Credits_enter() started MUSIC_CREDITS — without this it would freeze on its first note

            if (input->startPressed)
            {
                state = STATE_TITLE;
                Title_enter();
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
