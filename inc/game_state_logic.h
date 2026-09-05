#ifndef _GAME_STATE_LOGIC_H_
#define _GAME_STATE_LOGIC_H_

#include <stdbool.h>

// Hardware-free: no genesis.h, no SGDK calls. Compiles both under the
// m68k-elf cross-compiler and a native host compiler, so the transition
// table can be unit-tested on the host without an emulator.

typedef enum
{
    STATE_TITLE,
    STATE_GAME,
    STATE_BOSS,
    STATE_STAGE_CLEAR,
    STATE_NEXT_STAGE,
    STATE_PAUSE,
    STATE_PLAYER_HIT,
    STATE_PLAYER_DEAD,
    STATE_GAME_OVER,
    STATE_CONTINUE,
    STATE_ENDING,
    STATE_CREDITS
} GameState;

// Given the current state and whether START was just pressed this frame,
// returns the next state. States with no M01-defined transition yet pass
// through unchanged (later milestones extend this).
GameState GameState_computeNext(GameState current, bool startPressed);

#endif // _GAME_STATE_LOGIC_H_
