#ifndef _SCORE_H_
#define _SCORE_H_

#include <genesis.h>

void Score_reset(void);
void Score_add(u16 points);
u32 Score_get(void);

// SPEC.md §14/M14: the session's highest score reached so far. Never
// reset by Score_reset() (a new game shouldn't erase it) — only power-
// cycling the console does, since no save mechanism exists (NS-17).
u32 Score_getHigh(void);

#endif // _SCORE_H_
