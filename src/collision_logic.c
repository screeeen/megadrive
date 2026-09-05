#include "collision_logic.h"

bool Collision_overlaps(AABB a, AABB b)
{
    if (a.x + a.w <= b.x) return false;
    if (b.x + b.w <= a.x) return false;
    if (a.y + a.h <= b.y) return false;
    if (b.y + b.h <= a.y) return false;

    return true;
}
