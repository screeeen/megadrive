#include "combat.h"
#include "collision_logic.h"
#include "projectile.h"
#include "enemy.h"
#include "powerup.h"
#include "asteroid.h"
#include "score.h"
#include "combo.h"
#include "audio.h"
#include "explosion.h"

// Not specified in SPEC.md how often enemies drop power-ups — chosen here
// (see PROGRESS.md): every 3rd kill, cycling through all 8 types so each
// is reachable without a real per-type drop-chance table (that belongs to
// stage design, M07/M08, once there's a stage to tune it against).
#define POWERUP_DROP_INTERVAL 3

static u16 killCount;
static PowerupType nextDropType;

static AABB projectileBox(const Projectile* p)
{
    // laser_big is 16x8, not 8x8 like every other projectile; a minor
    // placeholder-era simplification (see PROGRESS.md) uses the small box
    // for it too rather than adding a per-type size table for one asset.
    AABB box = { p->x, p->y, PROJECTILE_SPRITE_W, PROJECTILE_SPRITE_H };
    return box;
}

static AABB enemyBox(const Enemy* e)
{
    AABB box = { e->x, e->y, e->spriteW, e->spriteH };
    return box;
}

static AABB powerupBox(const Powerup* p)
{
    AABB box = { p->x, p->y, POWERUP_SIZE, POWERUP_SIZE };
    return box;
}

static AABB asteroidBox(const Asteroid* a)
{
    AABB box = { a->x, a->y, a->spriteW, a->spriteH };
    return box;
}

static AABB playerBox(const Player* player)
{
    AABB box = { player->hitboxX, player->hitboxY, PLAYER_HITBOX_SIZE, PLAYER_HITBOX_SIZE };
    return box;
}

static AABB bossBox(const Boss* boss)
{
    AABB box = { boss->x, boss->y, BOSS_SPRITE_W, BOSS_SPRITE_H };
    return box;
}

static AABB bossVulnerablePointBox(const Boss* boss, u8 index)
{
    const VulnerablePoint* vp = &boss->def->vulnerablePoints[index];
    AABB box = { (s16) (boss->x + vp->offsetX), (s16) (boss->y + vp->offsetY), vp->w, vp->h };
    return box;
}

static void onEnemyKilled(u16 scoreGained, s16 x, s16 y)
{
    Score_add((u16) (scoreGained * Combo_getMultiplier()));
    Combo_onKill();
    Audio_playSfx(SFX_EXPLOSION);
    Explosion_spawn(x, y);

    if (++killCount >= POWERUP_DROP_INTERVAL)
    {
        killCount = 0;
        Powerup_spawn(nextDropType, x, y);
        nextDropType = (PowerupType) ((nextDropType + 1) % POWERUP_TYPE_COUNT);
    }
}

static void resolvePlayerBulletsVsEnemies(void)
{
    Projectile* projectiles = Projectile_getPool();
    Enemy* enemies = Enemy_getPool();

    for (u16 i = 0; i < PROJECTILE_POOL_SIZE; i++)
    {
        Projectile* p = &projectiles[i];

        if (!p->active || p->owner != PROJECTILE_OWNER_PLAYER)
            continue;

        for (u16 j = 0; j < ENEMY_POOL_SIZE; j++)
        {
            Enemy* e = &enemies[j];

            if (!e->active)
                continue;

            if (!Collision_overlaps(projectileBox(p), enemyBox(e)))
                continue;

            s16 deathX = e->x;
            s16 deathY = e->y;
            u16 scoreGained = Enemy_hit(e, p->damage);

            if (scoreGained > 0)
                onEnemyKilled(scoreGained, deathX, deathY);

            if (p->pierceRemaining > 0)
                p->pierceRemaining--;
            else
                Projectile_release(p);

            break; // this projectile is spent for the frame either way
        }
    }
}

static void resolvePlayerBulletsVsAsteroids(void)
{
    Projectile* projectiles = Projectile_getPool();
    Asteroid* asteroids = Asteroid_getPool();

    for (u16 i = 0; i < PROJECTILE_POOL_SIZE; i++)
    {
        Projectile* p = &projectiles[i];

        if (!p->active || p->owner != PROJECTILE_OWNER_PLAYER)
            continue;

        for (u16 j = 0; j < ASTEROID_POOL_SIZE; j++)
        {
            Asteroid* a = &asteroids[j];

            if (!a->active)
                continue;

            if (!Collision_overlaps(projectileBox(p), asteroidBox(a)))
                continue;

            // Large asteroids are indestructible (Asteroid_hit is a no-op
            // for them) — the shot still gets spent/pierced against it,
            // same as bumping into any other solid obstacle.
            s16 deathX = a->x;
            s16 deathY = a->y;
            u16 scoreGained = Asteroid_hit(a, p->damage);

            if (scoreGained > 0)
                onEnemyKilled(scoreGained, deathX, deathY);

            if (p->pierceRemaining > 0)
                p->pierceRemaining--;
            else
                Projectile_release(p);

            break;
        }
    }
}

static void resolveAsteroidContactVsPlayer(Player* player)
{
    Asteroid* asteroids = Asteroid_getPool();
    AABB pBox = playerBox(player);

    for (u16 i = 0; i < ASTEROID_POOL_SIZE; i++)
    {
        Asteroid* a = &asteroids[i];

        if (!a->active)
            continue;

        if (Collision_overlaps(asteroidBox(a), pBox))
            Player_hit(player); // no-op while already invulnerable/dead
    }
}

static void resolveEnemyBulletsVsPlayer(Player* player)
{
    Projectile* projectiles = Projectile_getPool();
    AABB pBox = playerBox(player);

    for (u16 i = 0; i < PROJECTILE_POOL_SIZE; i++)
    {
        Projectile* p = &projectiles[i];

        if (!p->active || p->owner != PROJECTILE_OWNER_ENEMY)
            continue;

        if (!Collision_overlaps(projectileBox(p), pBox))
            continue;

        Player_hit(player);
        Projectile_release(p);
    }
}

static void resolveEnemyContactVsPlayer(Player* player)
{
    Enemy* enemies = Enemy_getPool();
    AABB pBox = playerBox(player);

    for (u16 i = 0; i < ENEMY_POOL_SIZE; i++)
    {
        Enemy* e = &enemies[i];

        if (!e->active)
            continue;

        if (Collision_overlaps(enemyBox(e), pBox))
            Player_hit(player); // no-op while already invulnerable/dead
    }
}

static void applyPowerup(Player* player, PowerupType type)
{
    // 1UP gets its own distinct fanfare (SFX_1UP); every other pickup
    // shares one generic "powerup" jingle.
    Audio_playSfx(type == POWERUP_1UP ? SFX_1UP : SFX_POWERUP);

    switch (type)
    {
        case POWERUP_LASER:  Player_pickupWeapon(player, WEAPON_LASER);  break;
        case POWERUP_WIDE:   Player_pickupWeapon(player, WEAPON_WIDE);   break;
        case POWERUP_HOMING: Player_pickupWeapon(player, WEAPON_HOMING); break;
        case POWERUP_FLAME:  Player_pickupWeapon(player, WEAPON_FLAME);  break;
        case POWERUP_P:      Player_levelUpWeapon(player);               break;
        case POWERUP_BOMB:   Player_addBomb(player);                     break;
        case POWERUP_SPEED:  Player_applySpeedBoost(player);             break;
        case POWERUP_1UP:    Player_addLife(player);                     break;
        default: break;
    }
}

static void resolvePlayerVsPowerups(Player* player)
{
    Powerup* powerups = Powerup_getPool();
    AABB pBox = playerBox(player);

    for (u16 i = 0; i < POWERUP_POOL_SIZE; i++)
    {
        Powerup* p = &powerups[i];

        if (!p->active)
            continue;

        if (!Collision_overlaps(powerupBox(p), pBox))
            continue;

        applyPowerup(player, p->type);
        Powerup_release(p);
    }
}

static void resolvePlayerBulletsVsBoss(Boss* boss)
{
    if (!Boss_isActive(boss))
        return;

    Projectile* projectiles = Projectile_getPool();

    for (u16 i = 0; i < PROJECTILE_POOL_SIZE; i++)
    {
        Projectile* p = &projectiles[i];

        if (!p->active || p->owner != PROJECTILE_OWNER_PLAYER)
            continue;

        for (u8 v = 0; v < boss->def->vulnerablePointCount; v++)
        {
            if (!Collision_overlaps(projectileBox(p), bossVulnerablePointBox(boss, v)))
                continue;

            if (Boss_hit(boss, p->damage))
            {
                if (p->pierceRemaining > 0)
                    p->pierceRemaining--;
                else
                    Projectile_release(p);
            }

            break;
        }
    }
}

static void resolveBossContactVsPlayer(Boss* boss, Player* player)
{
    if (!Boss_isActive(boss))
        return;

    if (Collision_overlaps(bossBox(boss), playerBox(player)))
        Player_hit(player); // no-op while already invulnerable/dead
}

void Combat_resolveCollisions(Player* player, Boss* boss)
{
    resolvePlayerBulletsVsEnemies();
    resolveEnemyBulletsVsPlayer(player);
    resolveEnemyContactVsPlayer(player);
    resolvePlayerBulletsVsAsteroids();
    resolveAsteroidContactVsPlayer(player);
    resolvePlayerVsPowerups(player);
    resolvePlayerBulletsVsBoss(boss);
    resolveBossContactVsPlayer(boss, player);
}
