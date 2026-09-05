#include "weapon.h"
#include "projectile.h"
#include "resources.h"

// SPEC.md §7 gives relative behavior per weapon ("high cadence", "fast",
// "short range", "high damage"...) but no exact frame counts, speeds or
// damage values — chosen here and recorded in PROGRESS.md's decision log.

// --- Vulcan: L1=1/L2=2/L3=3 shots, cadence ramps with level ---
#define VULCAN_SPEED       6
#define VULCAN_SPREAD      6 // px offset between multi-shot projectiles
#define VULCAN_COOLDOWN_L1 10
#define VULCAN_COOLDOWN_L2 10
#define VULCAN_COOLDOWN_L3 6
#define VULCAN_DAMAGE_L1   1
#define VULCAN_DAMAGE_L2   2
#define VULCAN_DAMAGE_L3   2

// --- Laser: straight, fast, pierces, L3 is only visually bigger ---
#define LASER_SPEED     9
#define LASER_COOLDOWN  14
#define LASER_DAMAGE    3
#define LASER_PIERCE    2

// --- Wide: fan spread, more projectiles per level, low individual damage ---
#define WIDE_SPEED     6
#define WIDE_COOLDOWN  14
#define WIDE_DAMAGE    1
static const s8 wideOffsetsL1[3] = { -3, 0, 3 };
static const s8 wideOffsetsL2[5] = { -4, -2, 0, 2, 4 };
static const s8 wideOffsetsL3[7] = { -6, -4, -2, 0, 2, 4, 6 };

// --- Homing: single seeking missile, capped active count regardless of level ---
#define HOMING_SPEED        5
#define HOMING_COOLDOWN     20
#define HOMING_DAMAGE       2
#define HOMING_ACTIVE_LIMIT 4

// --- Flame: short burst, high damage, close-range by design ---
#define FLAME_SPEED     5
#define FLAME_COOLDOWN  8
#define FLAME_DAMAGE    4
#define FLAME_RANGE     56 // px of travel before it fizzles out

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
                              PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_VULCAN,
                              PROJECTILE_RANGE_UNLIMITED, PROJECTILE_PIERCE_NONE);
            cooldown = VULCAN_COOLDOWN_L1;
            break;

        case 2:
            Projectile_spawn(x, centerY - VULCAN_SPREAD, VULCAN_SPEED, 0, VULCAN_DAMAGE_L2,
                              PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_VULCAN,
                              PROJECTILE_RANGE_UNLIMITED, PROJECTILE_PIERCE_NONE);
            Projectile_spawn(x, centerY + VULCAN_SPREAD, VULCAN_SPEED, 0, VULCAN_DAMAGE_L2,
                              PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_VULCAN,
                              PROJECTILE_RANGE_UNLIMITED, PROJECTILE_PIERCE_NONE);
            cooldown = VULCAN_COOLDOWN_L2;
            break;

        default: // level 3
            Projectile_spawn(x, centerY - VULCAN_SPREAD, VULCAN_SPEED, 0, VULCAN_DAMAGE_L3,
                              PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_VULCAN,
                              PROJECTILE_RANGE_UNLIMITED, PROJECTILE_PIERCE_NONE);
            Projectile_spawn(x, centerY, VULCAN_SPEED, 0, VULCAN_DAMAGE_L3,
                              PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_VULCAN,
                              PROJECTILE_RANGE_UNLIMITED, PROJECTILE_PIERCE_NONE);
            Projectile_spawn(x, centerY + VULCAN_SPREAD, VULCAN_SPEED, 0, VULCAN_DAMAGE_L3,
                              PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_VULCAN,
                              PROJECTILE_RANGE_UNLIMITED, PROJECTILE_PIERCE_NONE);
            cooldown = VULCAN_COOLDOWN_L3;
            break;
    }
}

static void fireLaser(const Player* player)
{
    s16 x = player->x + PLAYER_SPRITE_W;
    s16 centerY = player->y + (PLAYER_SPRITE_H / 2);
    bool big = player->weaponLevel >= 3;

    // L3 swaps to the larger sprite (projectile.c's PROJECTILE_TYPE_LASER_BIG)
    // and pierces one more enemy; same speed/cooldown/base damage as L1/L2,
    // matching SPEC.md §7's "visually larger but technically cheap".
    Projectile_spawn(x, centerY, LASER_SPEED, 0, LASER_DAMAGE,
                      PROJECTILE_OWNER_PLAYER,
                      big ? PROJECTILE_TYPE_LASER_BIG : PROJECTILE_TYPE_LASER,
                      PROJECTILE_RANGE_UNLIMITED, big ? LASER_PIERCE + 1 : LASER_PIERCE);
    cooldown = LASER_COOLDOWN;
}

static void fireWide(const Player* player)
{
    s16 x = player->x + PLAYER_SPRITE_W;
    s16 centerY = player->y + (PLAYER_SPRITE_H / 2);
    const s8* offsets;
    u8 count;

    switch (player->weaponLevel)
    {
        case 1: offsets = wideOffsetsL1; count = 3; break;
        case 2: offsets = wideOffsetsL2; count = 5; break;
        default: offsets = wideOffsetsL3; count = 7; break;
    }

    for (u8 i = 0; i < count; i++)
    {
        Projectile_spawn(x, centerY, WIDE_SPEED, offsets[i], WIDE_DAMAGE,
                          PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_WIDE,
                          PROJECTILE_RANGE_UNLIMITED, PROJECTILE_PIERCE_NONE);
    }

    cooldown = WIDE_COOLDOWN;
}

static void fireHoming(const Player* player)
{
    if (Projectile_countActiveOfType(PROJECTILE_TYPE_HOMING) >= HOMING_ACTIVE_LIMIT)
    {
        cooldown = HOMING_COOLDOWN;
        return;
    }

    s16 x = player->x + PLAYER_SPRITE_W;
    s16 centerY = player->y + (PLAYER_SPRITE_H / 2);

    // No enemy list exists before M05, so there is nothing to seek yet:
    // flies straight until a target-selection hook can be wired in.
    Projectile_spawn(x, centerY, HOMING_SPEED, 0, HOMING_DAMAGE,
                      PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_HOMING,
                      PROJECTILE_RANGE_UNLIMITED, PROJECTILE_PIERCE_NONE);
    cooldown = HOMING_COOLDOWN;
}

static void fireFlame(const Player* player)
{
    s16 x = player->x + PLAYER_SPRITE_W;
    s16 centerY = player->y + (PLAYER_SPRITE_H / 2);

    Projectile_spawn(x, centerY, FLAME_SPEED, 0, FLAME_DAMAGE,
                      PROJECTILE_OWNER_PLAYER, PROJECTILE_TYPE_FLAME,
                      FLAME_RANGE, PROJECTILE_PIERCE_NONE);
    cooldown = FLAME_COOLDOWN;
}

void Weapon_update(Player* player, const InputState* input)
{
    if (cooldown > 0)
        cooldown--;

    if (!input->a || cooldown != 0)
        return;

    switch (player->weapon)
    {
        case WEAPON_VULCAN: fireVulcan(player); break;
        case WEAPON_LASER:  fireLaser(player);  break;
        case WEAPON_WIDE:   fireWide(player);   break;
        case WEAPON_HOMING: fireHoming(player); break;
        case WEAPON_FLAME:  fireFlame(player);  break;
        case WEAPON_COUNT:  break; // sentinel, never actually assigned to player->weapon
    }
}
