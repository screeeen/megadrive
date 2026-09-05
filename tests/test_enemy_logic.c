#include <stdio.h>
#include "enemy_logic.h"

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
    {
        EnemyHitResult out = Enemy_applyDamage((EnemyHitState){ 4 }, 1);
        CHECK(out.state.hp == 3, "partial damage reduces hp");
        CHECK(!out.destroyed, "partial damage does not destroy");
    }
    {
        EnemyHitResult out = Enemy_applyDamage((EnemyHitState){ 1 }, 1);
        CHECK(out.state.hp == 0, "lethal damage zeroes hp");
        CHECK(out.destroyed, "lethal damage destroys");
    }
    {
        EnemyHitResult out = Enemy_applyDamage((EnemyHitState){ 2 }, 5);
        CHECK(out.state.hp == 0, "overkill damage never goes negative");
        CHECK(out.destroyed, "overkill damage destroys");
    }

    if (failures == 0)
    {
        printf("PASS: all enemy_logic damage tests passed\n");
        return 0;
    }

    printf("FAIL: %d test(s) failed\n", failures);
    return 1;
}
