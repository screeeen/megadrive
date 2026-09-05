#ifndef _SCROLL_H_
#define _SCROLL_H_

#include <genesis.h>

// SPEC.md §14/§15: horizontal scroll + a parallax background layer.
// Genesis has exactly 2 tile planes (BG_A, BG_B); BG_A already hosts
// text/HUD (VDP_drawText's default plane), so BG_B is free for this.
// "3-4 layers" (SPEC.md §15) beyond that is stage-art content for M08+ —
// this milestone delivers the engine (an independently scrollable plane
// at a data-driven speed), demonstrated with one real layer.
// backgroundId selects the placeholder tile mix (StageDef.backgroundId):
// 0 = pure starfield (M07's testStage), 1 = starfield + a building
// silhouette (SPEC.md §16 Orbital City's "space"+"buildings" elements —
// skyline/traffic/defense towers are NOT implemented, see PROGRESS.md).
void Scroll_init(u8 backgroundId);

// Advances BG_B's horizontal scroll by `speed` px this frame (StageDef's
// scrollSpeed — SPEC.md §14: 2 normal, up to 4 max).
void Scroll_update(u8 speed);

#endif // _SCROLL_H_
