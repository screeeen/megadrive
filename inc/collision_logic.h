#ifndef _COLLISION_LOGIC_H_
#define _COLLISION_LOGIC_H_

#include <stdbool.h>

// Hardware-free, like game_state_logic.h / player_logic.h.

typedef struct
{
    short x;
    short y;
    short w;
    short h;
} AABB;

// Axis-aligned bounding box overlap test. Edge-touching boxes (a.x+a.w ==
// b.x) do NOT count as overlapping. SPEC.md §31: all collisions use
// independent hitboxes, checked deterministically every frame — this is
// that check. Not wired to anything yet (no enemies exist before M05);
// this is real, tested infrastructure ready for M05 to call.
bool Collision_overlaps(AABB a, AABB b);

#endif // _COLLISION_LOGIC_H_
