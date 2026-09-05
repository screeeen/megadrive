#ifndef _BOSS_LOGIC_H_
#define _BOSS_LOGIC_H_

#include <stdbool.h>

// Hardware-free, like enemy_logic.h/player_logic.h.

typedef struct
{
    short hp;
    unsigned char phase; // 1..3
} BossHitState;

typedef struct
{
    BossHitState state;
    bool phaseChanged;
    bool destroyed;
} BossHitResult;

// SPEC.md §22/§36: 3 phases, boss "must clearly show when it is
// vulnerable". Phase thresholds (phase 2 at <=2/3 maxHp, phase 3 at
// <=1/3 maxHp) aren't given numerically in SPEC.md — chosen here, see
// PROGRESS.md. Pure decision, no side effects — the caller (boss.c)
// applies the result and drives sprite/attack-pattern/state-machine
// effects from it.
BossHitResult Boss_applyDamage(BossHitState state, short maxHp, unsigned char damage);

#endif // _BOSS_LOGIC_H_
