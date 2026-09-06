#include "boss_data.h"
#include "enemy.h"
#include "asteroid.h"
#include "bullet_pattern.h"
#include "resources.h"

// SPEC.md §22 gives phase behaviors (cannons/drones/diagonal attacks) but
// not exact bullet speed/damage — chosen here, see PROGRESS.md.
#define BULLET_SPEED  4
#define BULLET_DAMAGE 2

static void orbitalGuardianPhase1(const Boss* boss) // "canones frontales"
{
    s16 fireX = boss->x;
    s16 fireY = (s16) (boss->y + (BOSS_SPRITE_H / 2));
    BulletPattern_diagonal(fireX, fireY, -1, 0, BULLET_SPEED, BULLET_DAMAGE);
}

static void orbitalGuardianPhase2(const Boss* boss) // "drones"
{
    Enemy_spawn(ENEMY_DRONE, boss->x, (s16) (boss->y + 4));
}

static void orbitalGuardianPhase3(const Boss* boss) // "ataques diagonales"
{
    s16 fireX = boss->x;
    s16 fireY = (s16) (boss->y + (BOSS_SPRITE_H / 2));
    BulletPattern_diagonal(fireX, fireY, -1, -1, BULLET_SPEED, BULLET_DAMAGE);
    BulletPattern_diagonal(fireX, fireY, -1,  1, BULLET_SPEED, BULLET_DAMAGE);
}

static const BossAttackFn orbitalGuardianPhaseAttacks[3] = {
    orbitalGuardianPhase1, orbitalGuardianPhase2, orbitalGuardianPhase3
};

// SPEC.md §36: "3-5 puntos vulnerables". Offsets/sizes are hand-picked
// sub-rectangles of bossGuardian's 32x32 sprite — see PROGRESS.md NS-11
// for the still-open "no per-point visual marker" caveat.
static const VulnerablePoint orbitalGuardianVulnerablePoints[3] = {
    { 4,  12, 8, 8 },
    { 12, 2,  8, 8 },
    { 20, 12, 8, 8 },
};

// SPEC.md §22: ~250 HP. SPEC.md §23: boss kill = 50,000 points.
const BossDef orbitalGuardianDef = {
    &bossGuardian,
    250,
    50000,
    orbitalGuardianVulnerablePoints,
    3,
    orbitalGuardianPhaseAttacks,
    3
};

// SPEC.md §17: "embestidas, proyectiles verticales, ataques desde ambos
// lados". The framework's boss movement is a straight INTRO approach only
// (no per-boss custom movement/burrowing) — see PROGRESS.md's M10 summary
// for why "entra y sale del terreno" is approximated with attack patterns
// alone rather than an unbuilt movement system.
static void sandWormPhase1(const Boss* boss) // "embestida" (a fast spray toward the player's lane)
{
    BulletPattern_burst(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)), BULLET_SPEED, BULLET_DAMAGE);
}

static void sandWormPhase2(const Boss* boss) // "proyectiles verticales"
{
    BulletPattern_topBottom(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)), BULLET_SPEED, BULLET_DAMAGE);
}

static void sandWormPhase3(const Boss* boss) // "ataques desde ambos lados" (approximated as all 4 directions)
{
    BulletPattern_cross(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)), BULLET_SPEED, BULLET_DAMAGE);
}

static const BossAttackFn sandWormPhaseAttacks[3] = {
    sandWormPhase1, sandWormPhase2, sandWormPhase3
};

static const VulnerablePoint sandWormVulnerablePoints[3] = {
    { 2,  6,  8, 6 },
    { 12, 14, 8, 6 },
    { 22, 22, 8, 6 },
};

// SPEC.md §40.3: ~350 HP effective. SPEC.md §23: boss kill = 50,000 points.
const BossDef sandWormDef = {
    &bossSandworm,
    350,
    50000,
    sandWormVulnerablePoints,
    3,
    sandWormPhaseAttacks,
    3
};

// SPEC.md §18: "brazos mecanicos" that "modifican la zona segura de la
// pantalla" — the framework has no dynamic safe-zone concept, so the arms
// are approximated as a stationary hazard (a spawned Turret) rather than
// faking a mechanic that doesn't exist yet; see PROGRESS.md's M10 summary.
static void industrialCorePhase1(const Boss* boss) // aimed cannon fire
{
    BulletPattern_aimed(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)),
                         boss->x - 100, boss->y, BULLET_SPEED, BULLET_DAMAGE);
}

static void industrialCorePhase2(const Boss* boss) // "brazo" hazard (approximated as a stationary Turret)
{
    Enemy_spawn(ENEMY_TURRET, boss->x, (s16) (boss->y + 4));
}

static void industrialCorePhase3(const Boss* boss) // peak intensity: aimed + cross
{
    BulletPattern_combined(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)),
                            boss->x - 100, boss->y, BULLET_SPEED, BULLET_DAMAGE);
}

static const BossAttackFn industrialCorePhaseAttacks[3] = {
    industrialCorePhase1, industrialCorePhase2, industrialCorePhase3
};

static const VulnerablePoint industrialCoreVulnerablePoints[3] = {
    { 8,  8,  8, 8 },
    { 16, 16, 8, 8 },
    { 4,  20, 8, 8 },
};

// SPEC.md §40.4: ~450 HP effective. SPEC.md §23: boss kill = 50,000 points.
const BossDef industrialCoreDef = {
    &bossIndustrialcore,
    450,
    50000,
    industrialCoreVulnerablePoints,
    3,
    industrialCorePhaseAttacks,
    3
};

// SPEC.md §19: "cañones, minas, drones, núcleo vulnerable". "Núcleo
// vulnerable" is just this framework's existing vulnerable-point system
// (nothing extra needed); "minas" are approximated by spawning a small
// drifting Asteroid — a reasonable reuse of M11's new hazard rather than a
// dedicated mine entity, since a mine is conceptually a small drifting
// hazard too. See PROGRESS.md's M11 summary.
static void miningFortressPhase1(const Boss* boss) // "canones"
{
    BulletPattern_diagonal(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)), -1, 0, BULLET_SPEED, BULLET_DAMAGE);
}

static void miningFortressPhase2(const Boss* boss) // "minas"
{
    Asteroid_spawn(ASTEROID_SMALL, boss->x, (s16) (boss->y + 4));
}

static void miningFortressPhase3(const Boss* boss) // "drones"
{
    Enemy_spawn(ENEMY_DRONE, boss->x, (s16) (boss->y + 4));
    Enemy_spawn(ENEMY_DRONE, boss->x, (s16) (boss->y + 20));
}

static const BossAttackFn miningFortressPhaseAttacks[3] = {
    miningFortressPhase1, miningFortressPhase2, miningFortressPhase3
};

static const VulnerablePoint miningFortressVulnerablePoints[3] = {
    { 13, 13, 6, 6 }, // the "núcleo vulnerable" itself (centered)
    { 4,  10, 6, 6 },
    { 22, 16, 6, 6 },
};

// SPEC.md §40.5: ~550 HP effective. SPEC.md §23: boss kill = 50,000 points.
const BossDef miningFortressDef = {
    &bossMiningfortress,
    550,
    50000,
    miningFortressVulnerablePoints,
    3,
    miningFortressPhaseAttacks,
    3
};

// SPEC.md §20 gives no explicit per-phase attack breakdown for Admiral X
// (unlike the earlier bosses) beyond "nave de mando" — phases are built
// from the same bullet-pattern building blocks, escalating in intensity
// like every other boss here, per MILESTONES.md M13's later "Final Boss"
// precedent of phases getting harder rather than merely different.
static void admiralXPhase1(const Boss* boss) // opening: aimed fire
{
    BulletPattern_aimed(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)),
                         boss->x - 100, boss->y, BULLET_SPEED, BULLET_DAMAGE);
}

static void admiralXPhase2(const Boss* boss) // escort: spawns a Fighter
{
    Enemy_spawn(ENEMY_FIGHTER, boss->x, (s16) (boss->y + 4));
}

static void admiralXPhase3(const Boss* boss) // peak: circular barrage
{
    BulletPattern_circular(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)), BULLET_SPEED, BULLET_DAMAGE);
}

static const BossAttackFn admiralXPhaseAttacks[3] = {
    admiralXPhase1, admiralXPhase2, admiralXPhase3
};

static const VulnerablePoint admiralXVulnerablePoints[3] = {
    { 4,  11, 6, 6 },
    { 14, 5,  6, 6 },
    { 22, 11, 6, 6 },
};

// SPEC.md §40.6: ~650 HP effective. SPEC.md §23: boss kill = 50,000 points.
const BossDef admiralXDef = {
    &bossAdmiralx,
    650,
    50000,
    admiralXVulnerablePoints,
    3,
    admiralXPhaseAttacks,
    3
};

// SPEC.md §21/§22/§40.8: Stage 6's "final boss entrance" (MILESTONES.md
// M12) built as a real, working boss using the generic framework — the
// same M08/M09 relationship repeated: M12 builds enough of Helios to be a
// genuine Stage 6 boss fight, M13 (Final Boss) adds what's uniquely
// final-boss-shaped and NOT required by M12's own acceptance list: the
// death sequence (explosion/scroll-stop), ENDING/CREDITS state transition
// (GameState already has these enum values, unused since M01), and phase
// 3's shrinking safe zone (no dynamic safe-zone system exists yet, same
// gap as Industrial Core's arms — NS-13). Until M13, Helios's death
// behaves like every other boss (STAGE_CLEAR -> NEXT_STAGE), not the real
// ending — deliberate and documented, not a faked completion.
static void heliosPhase1(const Boss* boss) // "canones, misiles, drones"
{
    BulletPattern_diagonal(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)), -1, 0, BULLET_SPEED, BULLET_DAMAGE);
    BulletPattern_aimed(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)),
                         boss->x - 100, boss->y, BULLET_SPEED, BULLET_DAMAGE); // "misiles": aimed, approximated (no true homing exists — NS-4)
    Enemy_spawn(ENEMY_DRONE, boss->x, (s16) (boss->y + 20));
}

static void heliosPhase2(const Boss* boss) // "patrones circulares, barridos, laseres"
{
    BulletPattern_circular(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)), BULLET_SPEED, BULLET_DAMAGE);
    BulletPattern_wave(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)), BULLET_SPEED, BULLET_DAMAGE); // "barridos"
}

static void heliosPhase3(const Boss* boss) // "nucleo expuesto... patrones finales" (safe-zone shrink NOT modeled — NS-13)
{
    BulletPattern_combined(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)),
                            boss->x - 100, boss->y, BULLET_SPEED, BULLET_DAMAGE);
    BulletPattern_cross(boss->x, (s16) (boss->y + (BOSS_SPRITE_H / 2)), BULLET_SPEED, BULLET_DAMAGE);
}

static const BossAttackFn heliosPhaseAttacks[3] = {
    heliosPhase1, heliosPhase2, heliosPhase3
};

// "Nucleo expuesto" (phase 3) is just this framework's existing
// vulnerable-point system — no extra work needed, same reading as every
// earlier boss's "vulnerable core".
static const VulnerablePoint heliosVulnerablePoints[3] = {
    { 13, 13, 6, 6 },
    { 4,  4,  4, 4 },
    { 24, 4,  4, 4 },
};

// No explicit "HP efectivo" is given for Helios in SPEC.md (§40.8
// deliberately avoids one: "no utilizar unicamente una barra de HP") —
// continuing the established +100-per-stage progression (250/350/450/
// 550/650) to 750 is the honest, documented choice here, same rationale
// as every other chosen-not-specified number in this project.
const BossDef heliosDef = {
    &bossHelios,
    750,
    50000,
    heliosVulnerablePoints,
    3,
    heliosPhaseAttacks,
    3
};

// StageDef.bossId -> BossDef lookup, so game_state.c doesn't need to know
// which specific boss belongs to which stage.
const BossDef* BossDef_forId(u8 bossId)
{
    switch (bossId)
    {
        case 2:  return &sandWormDef;
        case 3:  return &industrialCoreDef;
        case 4:  return &miningFortressDef;
        case 5:  return &admiralXDef;
        case 6:  return &heliosDef;
        default: return &orbitalGuardianDef; // 1: Orbital Guardian
    }
}
