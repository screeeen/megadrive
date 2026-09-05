#include "enemy.h"
#include "bullet_pattern.h"
#include "resources.h"

// SPEC.md §11 gives HP/behavior/(most) scores; exact speeds, cooldowns and
// Charger's score aren't specified there — chosen here, see PROGRESS.md.
#define DRONE_SPEED     -3
#define DRONE_HP         1
#define DRONE_SCORE      100

#define FIGHTER_SPEED   -2
#define FIGHTER_HP       2
#define FIGHTER_SCORE    200
#define FIGHTER_COOLDOWN 90
#define FIGHTER_DAMAGE   1

#define BOMBER_SPEED    -1
#define BOMBER_HP        4
#define BOMBER_SCORE     500
#define BOMBER_COOLDOWN  100
#define BOMBER_DAMAGE    1

#define TURRET_HP        3
#define TURRET_SCORE     300
#define TURRET_COOLDOWN  75
#define TURRET_DAMAGE    1

#define SWARM_SPEED     -3
#define SWARM_HP         1
#define SWARM_SCORE      100
#define SWARM_COOLDOWN   110
#define SWARM_DAMAGE     1

#define CHARGER_HP       2
#define CHARGER_SCORE    250 // not given an explicit value in SPEC.md
#define CHARGER_PAUSE    60
#define CHARGER_SPEED   -6

#define SHIELD_SPEED    -1
#define SHIELD_HP        6
#define SHIELD_SCORE     1000
#define SHIELD_COOLDOWN  100
#define SHIELD_DAMAGE    2
#define SHIELD_VULN_ON   60 // frames vulnerable
#define SHIELD_VULN_OFF  90 // frames shielded

#define MINIBOSS_SPEED    -1
#define MINIBOSS_HP        15  // not given numerically in SPEC.md; tougher than Shield's 6
#define MINIBOSS_SCORE     5000
#define MINIBOSS_COOLDOWN  50
#define MINIBOSS_DAMAGE    2

#define BULLET_SPEED     4

static Enemy pool[ENEMY_POOL_SIZE];

static const SpriteDefinition* spriteFor(EnemyType type)
{
    switch (type)
    {
        case ENEMY_FIGHTER: return &enemyFighter;
        case ENEMY_BOMBER:  return &enemyBomber;
        case ENEMY_TURRET:  return &enemyTurret;
        case ENEMY_SWARM:   return &enemySwarm;
        case ENEMY_CHARGER: return &enemyCharger;
        case ENEMY_SHIELD:  return &enemyShield;
        case ENEMY_MINIBOSS: return &enemyMiniboss;
        case ENEMY_DRONE:
        default:            return &enemyDrone;
    }
}

void Enemy_poolInit(void)
{
    for (u16 i = 0; i < ENEMY_POOL_SIZE; i++)
    {
        pool[i].active = FALSE;
        pool[i].sprite = NULL;
    }

    // All 7 enemy placeholders (and enemyBullet) were generated to share
    // one identical palette table on purpose, since several types can be
    // on screen at once — unlike player weapons, there is no single
    // "currently active type" to reload per spawn (see PROGRESS.md).
    PAL_setPalette(PAL3, enemyDrone.palette->data, DMA);
}

Enemy* Enemy_spawn(EnemyType type, s16 x, s16 y)
{
    for (u16 i = 0; i < ENEMY_POOL_SIZE; i++)
    {
        Enemy* e = &pool[i];

        if (e->active)
            continue;

        e->active = TRUE;
        e->type = type;
        e->state = ENEMY_STATE_ACTIVE;
        e->x = x;
        e->y = y;
        e->velocityY = 0;
        e->shootTimer = 0;
        e->stateTimer = 0;
        e->vulnerable = TRUE;

        if (type == ENEMY_BOMBER || type == ENEMY_MINIBOSS)
        {
            e->spriteW = 16;
            e->spriteH = 16;
        }
        else
        {
            e->spriteW = 8;
            e->spriteH = 8;
        }

        switch (type)
        {
            case ENEMY_DRONE:   e->velocityX = DRONE_SPEED;   e->hp = DRONE_HP;   e->scoreValue = DRONE_SCORE;   break;
            case ENEMY_FIGHTER: e->velocityX = FIGHTER_SPEED; e->hp = FIGHTER_HP; e->scoreValue = FIGHTER_SCORE; break;
            case ENEMY_BOMBER:  e->velocityX = BOMBER_SPEED;  e->hp = BOMBER_HP;  e->scoreValue = BOMBER_SCORE;  break;
            case ENEMY_TURRET:  e->velocityX = 0;             e->hp = TURRET_HP;  e->scoreValue = TURRET_SCORE;  break;
            case ENEMY_SWARM:   e->velocityX = SWARM_SPEED;   e->hp = SWARM_HP;   e->scoreValue = SWARM_SCORE;   break;
            case ENEMY_CHARGER: e->velocityX = 0;             e->hp = CHARGER_HP; e->scoreValue = CHARGER_SCORE;
                                 e->state = ENEMY_STATE_PAUSED;
                                 e->stateTimer = CHARGER_PAUSE;
                                 break;
            case ENEMY_SHIELD:  e->velocityX = SHIELD_SPEED;  e->hp = SHIELD_HP;  e->scoreValue = SHIELD_SCORE;
                                 e->stateTimer = SHIELD_VULN_ON;
                                 e->vulnerable = TRUE;
                                 break;
            case ENEMY_MINIBOSS: e->velocityX = MINIBOSS_SPEED; e->hp = MINIBOSS_HP; e->scoreValue = MINIBOSS_SCORE; break;
            default: e->velocityX = DRONE_SPEED; e->hp = DRONE_HP; e->scoreValue = DRONE_SCORE; break;
        }

        e->maxHp = e->hp;
        e->sprite = SPR_addSprite(spriteFor(type), x, y, TILE_ATTR(PAL3, TRUE, FALSE, FALSE));

        return e;
    }

    return NULL;
}

static void release(Enemy* e)
{
    e->active = FALSE;
    SPR_releaseSprite(e->sprite);
    e->sprite = NULL;
}

static void updateDrone(Enemy* e)
{
    (void) e; // no fire, straight line — velocity already set at spawn
}

static void updateFighter(Enemy* e, s16 playerX, s16 playerY)
{
    if (e->shootTimer > 0)
    {
        e->shootTimer--;
        return;
    }

    BulletPattern_aimed(e->x, e->y, playerX, playerY, BULLET_SPEED, FIGHTER_DAMAGE);
    e->shootTimer = FIGHTER_COOLDOWN;
}

static void updateBomber(Enemy* e)
{
    if (e->shootTimer > 0)
    {
        e->shootTimer--;
        return;
    }

    BulletPattern_burst(e->x, e->y, BULLET_SPEED, BOMBER_DAMAGE);
    e->shootTimer = BOMBER_COOLDOWN;
}

static void updateTurret(Enemy* e, s16 playerX, s16 playerY)
{
    if (e->shootTimer > 0)
    {
        e->shootTimer--;
        return;
    }

    BulletPattern_aimed(e->x, e->y, playerX, playerY, BULLET_SPEED, TURRET_DAMAGE);
    e->shootTimer = TURRET_COOLDOWN;
}

static void updateSwarm(Enemy* e)
{
    if (e->shootTimer > 0)
    {
        e->shootTimer--;
        return;
    }

    BulletPattern_wave(e->x, e->y, BULLET_SPEED, SWARM_DAMAGE);
    e->shootTimer = SWARM_COOLDOWN;
}

static void updateCharger(Enemy* e, s16 playerX, s16 playerY)
{
    if (e->state == ENEMY_STATE_PAUSED)
    {
        if (--e->stateTimer == 0)
        {
            // Aim: pick a straight-line velocity toward the player's
            // current position and start the dash.
            s16 dx = playerX - e->x;
            s16 dy = playerY - e->y;

            e->velocityX = (dx < 0) ? CHARGER_SPEED : (s16) -CHARGER_SPEED;
            e->velocityY = (dy == 0) ? 0 : ((dy < 0) ? (s16) -(-CHARGER_SPEED / 2) : (s16) (-CHARGER_SPEED / 2));
            e->state = ENEMY_STATE_CHARGING;
        }
    }
}

static void updateShield(Enemy* e)
{
    e->stateTimer--;

    if (e->stateTimer == 0)
    {
        e->vulnerable = !e->vulnerable;
        e->stateTimer = e->vulnerable ? SHIELD_VULN_ON : SHIELD_VULN_OFF;
    }

    if (e->shootTimer > 0)
    {
        e->shootTimer--;
    }
    else
    {
        BulletPattern_cross(e->x, e->y, BULLET_SPEED, SHIELD_DAMAGE);
        e->shootTimer = SHIELD_COOLDOWN;
    }
}

static void updateMiniboss(Enemy* e)
{
    if (e->shootTimer > 0)
    {
        e->shootTimer--;
        return;
    }

    BulletPattern_burst(e->x, e->y, BULLET_SPEED, MINIBOSS_DAMAGE);
    e->shootTimer = MINIBOSS_COOLDOWN;
}

void Enemy_poolUpdate(s16 playerX, s16 playerY)
{
    for (u16 i = 0; i < ENEMY_POOL_SIZE; i++)
    {
        Enemy* e = &pool[i];

        if (!e->active)
            continue;

        switch (e->type)
        {
            case ENEMY_DRONE:   updateDrone(e);                    break;
            case ENEMY_FIGHTER: updateFighter(e, playerX, playerY); break;
            case ENEMY_BOMBER:  updateBomber(e);                   break;
            case ENEMY_TURRET:  updateTurret(e, playerX, playerY);  break;
            case ENEMY_SWARM:   updateSwarm(e);                    break;
            case ENEMY_CHARGER: updateCharger(e, playerX, playerY); break;
            case ENEMY_SHIELD:  updateShield(e);                   break;
            case ENEMY_MINIBOSS: updateMiniboss(e);                 break;
            default: break;
        }

        e->x += e->velocityX;
        e->y += e->velocityY;

        if (e->x < -(e->spriteW) || e->x > 320 || e->y < -(e->spriteH) || e->y > 224)
        {
            release(e);
            continue;
        }

        SPR_setPosition(e->sprite, e->x, e->y);
    }
}

u16 Enemy_hit(Enemy* enemy, u8 damage)
{
    if (enemy->type == ENEMY_SHIELD && !enemy->vulnerable)
        return 0;

    EnemyHitResult out = Enemy_applyDamage((EnemyHitState){ enemy->hp }, damage);
    enemy->hp = out.state.hp;

    if (out.destroyed)
    {
        u16 score = enemy->scoreValue;
        release(enemy);
        return score;
    }

    return 0;
}

void Enemy_releaseAll(void)
{
    for (u16 i = 0; i < ENEMY_POOL_SIZE; i++)
    {
        if (pool[i].active)
            release(&pool[i]);
    }
}

Enemy* Enemy_getPool(void)
{
    return pool;
}
