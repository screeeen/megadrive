#include <stdio.h>
#include "player_logic.h"

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
    // Hit above weapon level 1: lose a weapon level, keep lives, no death.
    {
        PlayerHitState in = { 3, 3 };
        PlayerHitResult out = Player_applyHit(in);
        CHECK(out.state.weaponLevel == 2, "L3 hit -> L2");
        CHECK(out.state.lives == 3, "L3 hit keeps lives");
        CHECK(!out.lifeLost && !out.gameOver, "L3 hit is not lethal");
    }

    // Hit at weapon level 1 with lives remaining: lose a life, weapon stays L1.
    {
        PlayerHitState in = { 1, 3 };
        PlayerHitResult out = Player_applyHit(in);
        CHECK(out.state.weaponLevel == 1, "L1 hit keeps weapon at L1");
        CHECK(out.state.lives == 2, "L1 hit costs one life");
        CHECK(out.lifeLost && !out.gameOver, "L1 hit with lives left is not game over");
    }

    // Hit at weapon level 1 on the last life: game over.
    {
        PlayerHitState in = { 1, 1 };
        PlayerHitResult out = Player_applyHit(in);
        CHECK(out.state.lives == 0, "last-life L1 hit drains lives to 0");
        CHECK(out.lifeLost && out.gameOver, "last-life L1 hit is game over");
    }

    // Defensive: hit with 0 lives already (shouldn't normally happen) never
    // underflows.
    {
        PlayerHitState in = { 1, 0 };
        PlayerHitResult out = Player_applyHit(in);
        CHECK(out.state.lives == 0, "hit at 0 lives does not underflow");
        CHECK(out.gameOver, "hit at 0 lives is still game over");
    }

    // Weapon switching (B button): always advances and always resets to L1.
    {
        WeaponState in = { WEAPON_VULCAN, 3 };
        WeaponState out = Weapon_switchNext(in);
        CHECK(out.weapon == WEAPON_LASER, "VULCAN -> LASER");
        CHECK(out.weaponLevel == 1, "switching resets to L1 even from L3");
    }
    {
        WeaponState in = { WEAPON_FLAME, 2 };
        WeaponState out = Weapon_switchNext(in);
        CHECK(out.weapon == WEAPON_VULCAN, "the cycle wraps: FLAME -> VULCAN");
    }

    // Weapon level-up (same-weapon pickup): +1, capped at 3.
    {
        WeaponState in = { WEAPON_WIDE, 1 };
        WeaponState out = Weapon_levelUp(in);
        CHECK(out.weaponLevel == 2, "L1 -> L2");
        CHECK(out.weapon == WEAPON_WIDE, "level-up does not change weapon type");
    }
    {
        WeaponState in = { WEAPON_WIDE, 3 };
        WeaponState out = Weapon_levelUp(in);
        CHECK(out.weaponLevel == 3, "L3 is capped, does not overflow");
    }

    if (failures == 0)
    {
        printf("PASS: all player_logic hit-resolution/weapon tests passed\n");
        return 0;
    }

    printf("FAIL: %d test(s) failed\n", failures);
    return 1;
}
