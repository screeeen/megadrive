#ifndef _COMBAT_H_
#define _COMBAT_H_

#include "player.h"

// Resolves all SPEC.md §31 collisions for one frame: player projectiles vs.
// enemies (damage/destroy/score — closes out NS-M03-007/NS-M04's deferred
// item), enemy projectiles vs. player, and enemy-vs-player contact damage.
// Uses collision_logic.c's tested AABB check throughout.
void Combat_resolveCollisions(Player* player);

#endif // _COMBAT_H_
