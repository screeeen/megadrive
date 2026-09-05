#include "boss.h"
#include "enemy.h"
#include "bullet_pattern.h"
#include "resources.h"

// SPEC.md §22 gives phase behaviors (cannons/drones/diagonal attacks) and
// §40.8's overall timing budget, but not exact per-state frame counts —
// chosen here, see PROGRESS.md.
#define INTRO_FRAMES      60
#define ATTACK_FRAMES     120
#define ATTACK_FIRE_EVERY 30
#define TELEGRAPH_FRAMES  40
#define VULNERABLE_FRAMES 100
#define TRANSITION_FRAMES 40
#define DEATH_FRAMES      90

#define BOSS_BATTLE_X 250
#define BOSS_APPROACH_SPEED 2
#define BULLET_SPEED  4
#define BULLET_DAMAGE 2

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
    SPR_setVisibility(boss->spriteParts[0], HIDDEN);
    clearVulnerableText();
}

void Boss_spawn(Boss* boss, s16 x, s16 y, s16 maxHp, u16 score)
{
    boss->active = TRUE;
    boss->x = x;
    boss->y = y;
    boss->hp = maxHp;
    boss->maxHp = maxHp;
    boss->phase = 1;
    boss->score = score;
    boss->state = BOSS_STATE_INTRO;
    boss->timer = INTRO_FRAMES;

    // 3 vulnerable points (SPEC.md §36: "3-5 puntos vulnerables"), all
    // sharing the boss's single HP pool — see PROGRESS.md for why that's
    // a deliberate simplification rather than per-point HP tracking.
    boss->vulnerablePointCount = 3;
    boss->vulnerablePoints[0].offsetX = 4;  boss->vulnerablePoints[0].offsetY = 12; boss->vulnerablePoints[0].w = 8; boss->vulnerablePoints[0].h = 8;
    boss->vulnerablePoints[1].offsetX = 12; boss->vulnerablePoints[1].offsetY = 2;  boss->vulnerablePoints[1].w = 8; boss->vulnerablePoints[1].h = 8;
    boss->vulnerablePoints[2].offsetX = 20; boss->vulnerablePoints[2].offsetY = 12; boss->vulnerablePoints[2].w = 8; boss->vulnerablePoints[2].h = 8;

    // Shares PAL3 with enemies/power-ups/background — see PROGRESS.md's
    // palette-budget notes; every M05+ placeholder was generated to match.
    PAL_setPalette(PAL3, bossGuardian.palette->data, DMA);
    boss->spriteParts[0] = SPR_addSprite(&bossGuardian, x, y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));
}

static void fireAttackForPhase(const Boss* boss)
{
    s16 fireX = boss->x;
    s16 fireY = (s16) (boss->y + (BOSS_SPRITE_H / 2));

    switch (boss->phase)
    {
        case 1: // "canones frontales"
            BulletPattern_diagonal(fireX, fireY, -1, 0, BULLET_SPEED, BULLET_DAMAGE);
            break;

        case 2: // "drones"
            Enemy_spawn(ENEMY_DRONE, fireX, (s16) (boss->y + 4));
            break;

        default: // 3: "ataques diagonales"
            BulletPattern_diagonal(fireX, fireY, -1, -1, BULLET_SPEED, BULLET_DAMAGE);
            BulletPattern_diagonal(fireX, fireY, -1,  1, BULLET_SPEED, BULLET_DAMAGE);
            break;
    }
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
            if (--boss->timer == 0)
                enterAttack(boss);
            break;

        case BOSS_STATE_DEATH:
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
