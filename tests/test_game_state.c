#include <stdio.h>
#include "game_state_logic.h"

static int failures = 0;

#define CHECK(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("FAIL: %s\n", msg); \
            failures++; \
        } \
    } while (0)

int main(void)
{
    CHECK(GameState_computeNext(STATE_TITLE, false) == STATE_TITLE,
          "TITLE stays TITLE without START");
    CHECK(GameState_computeNext(STATE_TITLE, true) == STATE_GAME,
          "TITLE -> GAME on START");
    CHECK(GameState_computeNext(STATE_GAME, true) == STATE_PAUSE,
          "GAME -> PAUSE on START");
    CHECK(GameState_computeNext(STATE_PAUSE, true) == STATE_GAME,
          "PAUSE -> GAME on START (resume)");
    CHECK(GameState_computeNext(STATE_GAME, false) == STATE_GAME,
          "GAME stays GAME without START");
    CHECK(GameState_computeNext(STATE_BOSS, true) == STATE_BOSS,
          "states with no defined transition pass through unchanged");
    CHECK(GameState_computeNext(STATE_GAME_OVER, true) == STATE_GAME_OVER,
          "GAME_OVER has no START-driven transition here as of M06 (branches "
          "to CONTINUE or TITLE depending on continues remaining, handled "
          "outside computeNext, same reason as PLAYER_HIT below)");
    CHECK(GameState_computeNext(STATE_PLAYER_HIT, true) == STATE_PLAYER_HIT,
          "PLAYER_HIT has no START-driven transition (timer/lives-driven, handled outside computeNext)");

    if (failures == 0)
    {
        printf("PASS: all game_state transition tests passed\n");
        return 0;
    }

    printf("FAIL: %d test(s) failed\n", failures);
    return 1;
}
