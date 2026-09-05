#ifndef _COMBO_H_
#define _COMBO_H_

#include <genesis.h>

// SPEC.md §24 Combo: x1 -> x5, resets after a timeout with no kills.
void Combo_reset(void);

// Call once per GAME frame; counts the no-kill timeout down and resets the
// multiplier to x1 if it expires.
void Combo_update(void);

// Call once per enemy kill: raises the multiplier (capped at x5) and
// restarts the timeout.
void Combo_onKill(void);

// Current score multiplier, 1..5.
u8 Combo_getMultiplier(void);

#endif // _COMBO_H_
