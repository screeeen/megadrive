#include <stdio.h>
#include "boss_logic.h"

static int failures = 0;

#define CHECK(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("FAIL: %s\n", msg); \
            failures++; \
        } \
    } while (0)

int main(void)
{
    // maxHp = 250 (Orbital Guardian, SPEC.md §40.2): phase 2 at <=166, phase 3 at <=83.
    {
        BossHitResult out = Boss_applyDamage((BossHitState){ 250, 1 }, 250, 10);
        CHECK(out.state.hp == 240, "partial damage reduces hp");
        CHECK(out.state.phase == 1, "still phase 1 well above the threshold");
        CHECK(!out.phaseChanged && !out.destroyed, "no phase change, not destroyed");
    }
    {
        BossHitResult out = Boss_applyDamage((BossHitState){ 170, 1 }, 250, 10);
        CHECK(out.state.hp == 160, "hp crosses the phase-2 threshold (<=166)");
        CHECK(out.state.phase == 2, "enters phase 2");
        CHECK(out.phaseChanged, "phase change reported");
    }
    {
        BossHitResult out = Boss_applyDamage((BossHitState){ 90, 2 }, 250, 10);
        CHECK(out.state.hp == 80, "hp crosses the phase-3 threshold (<=83)");
        CHECK(out.state.phase == 3, "enters phase 3");
        CHECK(out.phaseChanged, "phase change reported");
    }
    {
        BossHitResult out = Boss_applyDamage((BossHitState){ 5, 3 }, 250, 10);
        CHECK(out.state.hp == 0, "overkill damage never goes negative");
        CHECK(out.destroyed, "destroyed once hp reaches 0");
    }
    {
        // Already in phase 2 (hp already <=166, consistent with how phase 2
        // was reached); more damage that stays above the phase-3 line
        // (>83) should not report a spurious change.
        BossHitResult out = Boss_applyDamage((BossHitState){ 160, 2 }, 250, 5);
        CHECK(out.state.phase == 2, "stays phase 2");
        CHECK(!out.phaseChanged, "no phase-change noise when nothing crossed a threshold");
    }

    if (failures == 0)
    {
        printf("PASS: all boss_logic damage/phase tests passed\n");
        return 0;
    }

    printf("FAIL: %d test(s) failed\n", failures);
    return 1;
}
