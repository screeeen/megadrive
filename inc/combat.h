#ifndef _COMBAT_H_
#define _COMBAT_H_

#include "player.h"
#include "boss.h"

// Resolves all SPEC.md §31 collisions for one frame: player projectiles vs.
// enemies (damage/destroy/score), enemy projectiles vs. player,
// enemy-vs-player contact damage, player vs. power-ups, and (M08) player
// projectiles vs. the boss's vulnerable points + boss-vs-player contact.
// `boss` is checked via Boss_isActive() internally, so it's safe to pass
// even when no boss encounter is underway. Uses collision_logic.c's
// tested AABB check throughout.
void Combat_resolveCollisions(Player* player, Boss* boss);

#endif // _COMBAT_H_
