#include <stdio.h>
#include "collision_logic.h"

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
    AABB a = { 0, 0, 6, 6 };

    CHECK(Collision_overlaps(a, (AABB){ 3, 3, 6, 6 }), "partial overlap detected");
    CHECK(Collision_overlaps(a, (AABB){ 0, 0, 6, 6 }), "identical boxes overlap");
    CHECK(Collision_overlaps(a, (AABB){ -3, -3, 6, 6 }), "overlap from the other side");
    CHECK(!Collision_overlaps(a, (AABB){ 6, 0, 6, 6 }), "edge-touching on X is not an overlap");
    CHECK(!Collision_overlaps(a, (AABB){ 0, 6, 6, 6 }), "edge-touching on Y is not an overlap");
    CHECK(!Collision_overlaps(a, (AABB){ 100, 100, 6, 6 }), "disjoint boxes do not overlap");
    CHECK(Collision_overlaps(a, (AABB){ 2, 2, 1, 1 }), "small box fully inside a large one overlaps");

    if (failures == 0)
    {
        printf("PASS: all collision_logic overlap tests passed\n");
        return 0;
    }

    printf("FAIL: %d test(s) failed\n", failures);
    return 1;
}
