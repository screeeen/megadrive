#ifndef _SCORE_H_
#define _SCORE_H_

#include <genesis.h>

// Minimal score tracking so NS-M05's "scoring works" is real and visible
// now, via the debug HUD. The full HUD format (SPEC.md §22: SCORE/HI/etc.)
// is M14's job — this is deliberately not that, just Score_add/Score_get.
void Score_reset(void);
void Score_add(u16 points);
u32 Score_get(void);

#endif // _SCORE_H_
