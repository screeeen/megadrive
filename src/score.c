#include "score.h"

static u32 score;
static u32 highScore;

void Score_reset(void)
{
    score = 0;
}

void Score_add(u16 points)
{
    score += points;

    if (score > highScore)
        highScore = score;
}

u32 Score_get(void)
{
    return score;
}

u32 Score_getHigh(void)
{
    return highScore;
}
