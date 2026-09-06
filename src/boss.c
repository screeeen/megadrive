#include "boss.h"
#include "explosion.h"

// SPEC.md §40.8 gives an overall timing budget but not exact per-state
// frame counts — chosen here, see PROGRESS.md. Shared by every boss for
// now (not yet a per-BossDef field: no two bosses exist yet to show that
// this needs to vary — see PROGRESS.md).
#define INTRO_FRAMES      60
#define ATTACK_FRAMES     120
#define ATTACK_FIRE_EVERY 30
#define TELEGRAPH_FRAMES  40
#define VULNERABLE_FRAMES 100
#define TRANSITION_FRAMES 40
#define DEATH_FRAMES      90
#define DEATH_HIDE_FRAMES 20 // final stretch of DEATH: fully hidden, no more strobe

#define BOSS_BATTLE_X 250
#define BOSS_APPROACH_SPEED 2

// SPEC.md §36: "mostrar claramente cuando es vulnerable". ATTACK and
// VULNERABLE otherwise render as the identical solid, fully-visible sprite
// (TELEGRAPH's blink is the only cue, and it ends the instant VULNERABLE
// starts) — a player has no ongoing way to tell the two apart, so hits
// landed during ATTACK/TRANSITION silently do nothing and the boss can look
// unkillable. This text is the fix: on screen only for the actual
// VULNERABLE window.
static void clearVulnerableText(void)
{
    VDP_clearText(11, 4, 11);
}

static void enterAttack(Boss* boss)
{
    boss->state = BOSS_STATE_ATTACK;
    boss->timer = ATTACK_FRAMES;
    clearVulnerableText();

    // TRANSITION's new phase-change blink (M16) can leave the sprite
    // HIDDEN on whichever frame its timer happens to hit 0 — restore it
    // here so ATTACK never silently starts invisible.
    SPR_setVisibility(boss->spriteParts[0], VISIBLE);
}

static void enterTelegraph(Boss* boss)
{
    boss->state = BOSS_STATE_TELEGRAPH;
    boss->timer = TELEGRAPH_FRAMES;
}

static void enterVulnerable(Boss* boss)
{
    boss->state = BOSS_STATE_VULNERABLE;
    boss->timer = VULNERABLE_FRAMES;
    SPR_setVisibility(boss->spriteParts[0], VISIBLE);
    VDP_drawText("VULNERABLE!", 11, 4);
}

static void enterTransition(Boss* boss)
{
    boss->state = BOSS_STATE_TRANSITION;
    boss->timer = TRANSITION_FRAMES;
    SPR_setVisibility(boss->spriteParts[0], VISIBLE);
    clearVulnerableText();
}

static void enterDeath(Boss* boss)
{
    boss->state = BOSS_STATE_DEATH;
    boss->timer = DEATH_FRAMES;
    clearVulnerableText();
}

void Boss_spawn(Boss* boss, const BossDef* def, s16 x, s16 y)
{
    // Defensive, same shape as the M07 pool bugfix (PROGRESS.md): if a
    // previous encounter's sprite was somehow never released before this
    // call, SPR_addSprite would allocate a second slot while this pointer
    // gets silently overwritten below, orphaning the old one in SGDK's
    // sprite engine instead of leaking cleanly.
    if (boss->spriteParts[0])
        SPR_releaseSprite(boss->spriteParts[0]);

    boss->active = TRUE;
    boss->x = x;
    boss->y = y;
    boss->hp = def->maxHp;
    boss->maxHp = def->maxHp;
    boss->phase = 1;
    boss->score = def->score;
    boss->def = def;
    boss->state = BOSS_STATE_INTRO;
    boss->timer = INTRO_FRAMES;

    // Shares PAL3 with enemies/power-ups/background — see PROGRESS.md's
    // palette-budget notes; every M05+ placeholder was generated to match.
    PAL_setPalette(PAL3, def->sprite->palette->data, DMA);

    // SPR_addSprite (plain) can fail on VRAM fragmentation per SGDK's own
    // header docs — SPR_addSpriteSafe retries once after SPR_defragVRAM().
    // The boss is by far the largest single sprite in the game (32x32, 16
    // tiles) and is repeatedly spawned/released across a play session
    // (once per stage loop), making it the most fragmentation-sensitive
    // allocation here: a failed plain alloc silently leaves spriteParts[0]
    // NULL, and every later SPR_setPosition/SPR_setVisibility call on it
    // corrupts SGDK's sprite engine state, crashing much later and
    // somewhere unrelated (see PROGRESS.md's M09 crash investigation).
    boss->spriteParts[0] = SPR_addSpriteSafe(def->sprite, x, y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
}

static void fireAttackForPhase(const Boss* boss)
{
    const BossDef* def = boss->def;

    if (boss->phase == 0 || boss->phase > def->phaseCount)
        return;

    def->phaseAttacks[boss->phase - 1](boss);
}

void Boss_update(Boss* boss, s16 playerX, s16 playerY)
{
    (void) playerX;
    (void) playerY;

    if (!boss->active)
        return;

    switch (boss->state)
    {
        case BOSS_STATE_INTRO:
            if (boss->x > BOSS_BATTLE_X)
                boss->x -= BOSS_APPROACH_SPEED;

            if (--boss->timer == 0 || boss->x <= BOSS_BATTLE_X)
            {
                boss->x = BOSS_BATTLE_X;
                enterAttack(boss);
            }
            break;

        case BOSS_STATE_ATTACK:
            if ((boss->timer % ATTACK_FIRE_EVERY) == 0)
                fireAttackForPhase(boss);

            if (--boss->timer == 0)
                enterTelegraph(boss);
            break;

        case BOSS_STATE_TELEGRAPH:
        {
            bool blinkVisible = ((boss->timer / 8) & 1) == 0;
            SPR_setVisibility(boss->spriteParts[0], blinkVisible ? VISIBLE : HIDDEN);

            if (--boss->timer == 0)
                enterVulnerable(boss);
            break;
        }

        case BOSS_STATE_VULNERABLE:
            if (--boss->timer == 0)
                enterAttack(boss);
            break;

        case BOSS_STATE_TRANSITION:
        {
            // M16: a fast blink (distinct from TELEGRAPH's slower one) as
            // the "phase changed" warning — TRANSITION previously looked
            // identical to a solid ATTACK/VULNERABLE boss.
            bool blinkVisible = ((boss->timer / 4) & 1) == 0;
            SPR_setVisibility(boss->spriteParts[0], blinkVisible ? VISIBLE : HIDDEN);

            if (--boss->timer == 0)
                enterAttack(boss);
            break;
        }

        case BOSS_STATE_DEATH:
            // A fast strobe of the boss's own sprite (M09), now paired
            // with real explosion sprites (M16's explosion.c) spawned at
            // scattered points across its footprint every 15 frames — a
            // proper death sequence instead of just the strobe alone.
            // Settles into fully hidden for the final stretch before the
            // STAGE_CLEAR/ENDING transition.
            if (boss->timer > DEATH_HIDE_FRAMES)
            {
                SPR_setVisibility(boss->spriteParts[0], ((boss->timer / 4) & 1) ? VISIBLE : HIDDEN);

                if ((boss->timer % 15) == 0)
                {
                    s16 offsetX = (s16) ((boss->timer * 7) % BOSS_SPRITE_W);
                    s16 offsetY = (s16) ((boss->timer * 5) % BOSS_SPRITE_H);
                    Explosion_spawn((s16) (boss->x + offsetX), (s16) (boss->y + offsetY));
                }
            }
            else
                SPR_setVisibility(boss->spriteParts[0], HIDDEN);

            if (--boss->timer == 0)
                boss->active = FALSE;
            break;

        default:
            break;
    }

    if (boss->state != BOSS_STATE_DEATH)
        SPR_setPosition(boss->spriteParts[0], boss->x, boss->y);
}

bool Boss_hit(Boss* boss, u8 damage)
{
    if (!boss->active || boss->state != BOSS_STATE_VULNERABLE)
        return FALSE;

    BossHitState in;
    in.hp = boss->hp;
    in.phase = boss->phase;

    BossHitResult out = Boss_applyDamage(in, boss->maxHp, damage);
    boss->hp = out.state.hp;

    if (out.destroyed)
    {
        enterDeath(boss);
        return TRUE;
    }

    if (out.phaseChanged)
    {
        boss->phase = out.state.phase;
        enterTransition(boss);
    }

    return TRUE;
}

bool Boss_isActive(const Boss* boss)
{
    return boss->active;
}

bool Boss_isEncounterOver(const Boss* boss)
{
    return boss->state == BOSS_STATE_DEATH && !boss->active;
}

void Boss_release(Boss* boss)
{
    if (boss->spriteParts[0])
    {
        SPR_releaseSprite(boss->spriteParts[0]);
        boss->spriteParts[0] = NULL;
    }

    boss->active = FALSE;
}
