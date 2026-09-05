#include "weapon.h"
#include "projectile.h"

// SPEC.md §7 Vulcan: L1 = 1 shot/high cadence/low damage, L2 = 2 shots/
// higher damage (cadence unchanged), L3 = 3 shots/maximum cadence. Exact
// frame counts and damage values aren't specified numerically there —
// chosen here and recorded in PROGRESS.md's decision log.
#define VULCAN_SPEED       6
#define VULCAN_SPREAD      6 // px offset between multi-shot projectiles
#define VULCAN_COOLDOWN_L1 10
#define VULCAN_COOLDOWN_L2 10
#define VULCAN_COOLDOWN_L3 6
#define VULCAN_DAMAGE_L1   1
#define VULCAN_DAMAGE_L2   2
#define VULCAN_DAMAGE_L3   2

static u16 cooldown;

void Weapon_init(void)
{
    cooldown = 0;
}

static void fireVulcan(const Player* player)
{
    s16 x = player->x + PLAYER_SPRITE_W;
    s16 centerY = player->y + (PLAYER_SPRITE_H / 2);

    switch (player->weaponLevel)
    {
        case 1:
            Projectile_spawn(x, centerY, VULCAN_SPEED, 0, VULCAN_DAMAGE_L1,
                              PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_VULCAN);
            cooldown = VULCAN_COOLDOWN_L1;
            break;

        case 2:
            Projectile_spawn(x, centerY - VULCAN_SPREAD, VULCAN_SPEED, 0, VULCAN_DAMAGE_L2,
                              PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_VULCAN);
            Projectile_spawn(x, centerY + VULCAN_SPREAD, VULCAN_SPEED, 0, VULCAN_DAMAGE_L2,
                              PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_VULCAN);
            cooldown = VULCAN_COOLDOWN_L2;
            break;

        default: // level 3
            Projectile_spawn(x, centerY - VULCAN_SPREAD, VULCAN_SPEED, 0, VULCAN_DAMAGE_L3,
                              PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_VULCAN);
            Projectile_spawn(x, centerY, VULCAN_SPEED, 0, VULCAN_DAMAGE_L3,
                              PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_VULCAN);
            Projectile_spawn(x, centerY + VULCAN_SPREAD, VULCAN_SPEED, 0, VULCAN_DAMAGE_L3,
                              PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_VULCAN);
            cooldown = VULCAN_COOLDOWN_L3;
            break;
    }
}

void Weapon_update(Player* player, const InputState* input)
{
    if (cooldown > 0)
        cooldown--;

    if (input->a && cooldown == 0)
        fireVulcan(player);
}
