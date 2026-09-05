#ifndef _WEAPON_H_
#define _WEAPON_H_

#include "player.h"
#include "input.h"

void Weapon_init(void);

// Fires Vulcan (the only weapon until M04) on a held A, gated by a
// per-level cooldown. Spawns into the Projectile pool; a full pool just
// silently drops the shot (see projectile.h).
void Weapon_update(Player* player, const InputState* input);

#endif // _WEAPON_H_
