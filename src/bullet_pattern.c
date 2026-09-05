#include "bullet_pattern.h"
#include "projectile.h"

#define ENEMY_BULLET_RANGE PROJECTILE_RANGE_UNLIMITED
#define ENEMY_BULLET_PIERCE PROJECTILE_PIERCE_NONE

static void spawn(s16 x, s16 y, s16 vx, s16 vy, u8 damage)
{
    Projectile_spawn(x, y, vx, vy, damage, PROJECTILE_OWNER_ENEMY,
                      PROJECTILE_TYPE_ENEMY_BULLET, ENEMY_BULLET_RANGE, ENEMY_BULLET_PIERCE);
}

void BulletPattern_aimed(s16 x, s16 y, s16 targetX, s16 targetY, s16 speed, u8 damage)
{
    s16 dx = targetX - x;
    s16 dy = targetY - y;

    // Cheap direction bias, not a true normalized vector (no FPU): always
    // heads toward the player horizontally, and leans vertically toward
    // them proportionally to how far off-axis they are, clamped to +-speed.
    s16 vy = 0;

    if (dx != 0)
        vy = (s16) ((dy * speed) / (dx < 0 ? -dx : dx));

    if (vy > speed) vy = speed;
    if (vy < -speed) vy = -speed;

    spawn(x, y, (s16) -speed, vy, damage);
}

void BulletPattern_diagonal(s16 x, s16 y, s8 dirX, s8 dirY, s16 speed, u8 damage)
{
    spawn(x, y, (s16) (dirX * speed), (s16) (dirY * speed), damage);
}

void BulletPattern_circular(s16 x, s16 y, s16 speed, u8 damage)
{
    // 8-direction compass LUT, no trig.
    static const s8 dir[8][2] = {
        {  1,  0 }, {  1,  1 }, {  0,  1 }, { -1,  1 },
        { -1,  0 }, { -1, -1 }, {  0, -1 }, {  1, -1 }
    };

    for (u8 i = 0; i < 8; i++)
        spawn(x, y, (s16) (dir[i][0] * speed), (s16) (dir[i][1] * speed), damage);
}

void BulletPattern_topBottom(s16 x, s16 y, s16 speed, u8 damage)
{
    spawn(x, y, (s16) -speed, (s16) -speed, damage); // toward top-left
    spawn(x, y, (s16) -speed, speed, damage);        // toward bottom-left
}

void BulletPattern_burst(s16 x, s16 y, s16 speed, u8 damage)
{
    spawn(x, y, (s16) -speed, -2, damage);
    spawn(x, y, (s16) -speed,  0, damage);
    spawn(x, y, (s16) -speed,  2, damage);
}

void BulletPattern_wave(s16 x, s16 y, s16 speed, u8 damage)
{
    spawn(x, y, (s16) -speed, 2, damage);
}

void BulletPattern_cross(s16 x, s16 y, s16 speed, u8 damage)
{
    spawn(x, y, (s16) -speed, 0, damage);
    spawn(x, y, speed, 0, damage);
    spawn(x, y, 0, (s16) -speed, damage);
    spawn(x, y, 0, speed, damage);
}

void BulletPattern_combined(s16 x, s16 y, s16 targetX, s16 targetY, s16 speed, u8 damage)
{
    BulletPattern_aimed(x, y, targetX, targetY, speed, damage);
    BulletPattern_cross(x, y, speed, damage);
}
