#ifndef _BOMB_H_
#define _BOMB_H_

#include "player.h"

// SPEC.md §9: consumes one bomb (no-op / returns false if none held),
// clears every active enemy projectile, deals high damage to every active
// enemy (including bypassing Shield's directional resistance — a bomb is
// an emergency tool, not a normal attack), and grants ~0.5s of player
// invulnerability. The caller (game_state.c) is responsible for any
// explosion/feedback visual.
bool Bomb_use(Player* player);

#endif // _BOMB_H_
