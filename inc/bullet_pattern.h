#ifndef _BULLET_PATTERN_H_
#define _BULLET_PATTERN_H_

#include <genesis.h>

// TASKS.md NS-M05-011's 8 named patterns, as small reusable building blocks
// over Projectile_spawn (owner=ENEMY, type=PROJECTILE_TYPE_ENEMY_BULLET).
// No trig/FPU: directions come from small fixed tables, matching real
// Genesis constraints (AGENT.md §8's "no premature optimization" cuts the
// other way here — a LUT *is* the appropriate embedded technique).
//
// Wired to a real enemy now: aimed (Fighter/Turret), burst (Bomber), wave
// (Swarm), cross (Shield, when vulnerable). diagonal/circular/top_bottom/
// combined are implemented and visually inspectable but not yet fired by
// any M05 enemy — ready for M07/M08 stage design. See PROGRESS.md.

// Single shot roughly toward (targetX, targetY), speed clamped so it's
// cheap (no division-heavy true angle math).
void BulletPattern_aimed(s16 x, s16 y, s16 targetX, s16 targetY, s16 speed, u8 damage);

// Single shot along a fixed diagonal (dirX/dirY are -1/0/1).
void BulletPattern_diagonal(s16 x, s16 y, s8 dirX, s8 dirY, s16 speed, u8 damage);

// N shots evenly spread in a circle (8-direction compass LUT).
void BulletPattern_circular(s16 x, s16 y, s16 speed, u8 damage);

// Two shots from one point, arcing toward the top and bottom of the screen.
void BulletPattern_topBottom(s16 x, s16 y, s16 speed, u8 damage);

// 3 shots in the same general direction with a small spread (a quick spray,
// not sustained fire — the caller's own cooldown controls repetition).
void BulletPattern_burst(s16 x, s16 y, s16 speed, u8 damage);

// Single shot with a modest diagonal bias, approximating an oscillating
// path without per-projectile sinusoidal state (SGDK has no FPU).
void BulletPattern_wave(s16 x, s16 y, s16 speed, u8 damage);

// 4 shots in the cardinal directions.
void BulletPattern_cross(s16 x, s16 y, s16 speed, u8 damage);

// Aimed shot plus a cross burst — a small demonstration of combining two
// patterns from one call site, per TASKS.md's "combined" entry.
void BulletPattern_combined(s16 x, s16 y, s16 targetX, s16 targetY, s16 speed, u8 damage);

#endif // _BULLET_PATTERN_H_
