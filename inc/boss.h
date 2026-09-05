#ifndef _BOSS_H_
#define _BOSS_H_

#include <genesis.h>
#include "boss_logic.h"

// TASKS.md NS-M09-002.
typedef enum
{
    BOSS_STATE_INACTIVE,
    BOSS_STATE_INTRO,
    BOSS_STATE_ATTACK,
    BOSS_STATE_TELEGRAPH,
    BOSS_STATE_VULNERABLE,
    BOSS_STATE_TRANSITION,
    BOSS_STATE_DEATH
} BossState;

#define BOSS_SPRITE_W 32
#define BOSS_SPRITE_H 32
#define BOSS_MAX_VULNERABLE_POINTS 3

// A rectangular sub-region of the boss's bounding box that can be hit
// independently (TASKS.md NS-M09-003). All points currently share the
// boss's single HP pool — see PROGRESS.md for why that's a deliberate
// simplification rather than per-point HP.
typedef struct
{
    s16 offsetX;
    s16 offsetY;
    s16 w;
    s16 h;
} VulnerablePoint;

// TASKS.md NS-M09-001. `spriteParts` is an array so a future boss can
// compose several hardware sprites (SPEC.md §31: "los graficos grandes
// deben construirse utilizando bloques reutilizables"); the Orbital
// Guardian uses just one (32x32, the largest single Genesis hardware
// sprite) — see PROGRESS.md.
typedef struct
{
    bool active;
    s16 x;
    s16 y;
    s16 hp;
    s16 maxHp;
    u8 phase;
    u16 timer; // meaning depends on `state`
    BossState state;

    VulnerablePoint vulnerablePoints[BOSS_MAX_VULNERABLE_POINTS];
    u8 vulnerablePointCount;

    u16 score;
    Sprite* spriteParts[1];
} Boss;

// Starts the boss's INTRO at (x, y) with the given max HP/score. Only one
// boss is modeled at a time (module-level singleton, like Player) — no
// stage has more than one active boss.
void Boss_spawn(Boss* boss, s16 x, s16 y, s16 maxHp, u16 score);

// Advances the state machine (attack pattern firing, telegraph, vulnerable
// window, phase transitions) given the player's position for aimed
// attacks.
void Boss_update(Boss* boss, s16 playerX, s16 playerY);

// Applies damage; no-op (returns false) unless the boss is in
// BOSS_STATE_VULNERABLE. Death is not instantaneous (SPEC.md wants an
// explosion/feedback beat first) — the caller reads Boss_isEncounterOver()
// each frame and awards boss->score once that beat finishes.
bool Boss_hit(Boss* boss, u8 damage);

bool Boss_isActive(const Boss* boss);
bool Boss_isEncounterOver(const Boss* boss); // DEATH sequence finished
void Boss_release(Boss* boss);

#endif // _BOSS_H_
