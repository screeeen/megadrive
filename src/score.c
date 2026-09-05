#include "score.h"

static u32 score;

void Score_reset(void)
{
    score = 0;
}

void Score_add(u16 points)
{
    score += points;
}

u32 Score_get(void)
{
    return score;
}
