#include "scroll.h"
#include "resources.h"

#define PLANE_WIDTH_TILES  64
#define PLANE_HEIGHT_TILES 32

#define TILE_STAR    TILE_USER_INDEX
#define TILE_BUILDING (TILE_USER_INDEX + 1)

static s16 scrollX;

void Scroll_init(u8 backgroundId)
{
    scrollX = 0;

    VDP_setPlaneSize(PLANE_WIDTH_TILES, PLANE_HEIGHT_TILES, TRUE);
    VDP_loadTileSet(&bgStar, TILE_STAR, DMA);

    u16 starAttr = TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, TILE_STAR);
    VDP_fillTileMapRect(BG_B, starAttr, 0, 0, PLANE_WIDTH_TILES, PLANE_HEIGHT_TILES);

    if (backgroundId == 1)
    {
        // Stage 1 (Orbital City): starfield plus a sparse building
        // silhouette column every few tiles, on the lower half of the
        // plane (a horizon-like effect). Skyline/traffic/defense-towers
        // (NS-M08-001) are NOT implemented — see PROGRESS.md.
        VDP_loadTileSet(&bgBuilding, TILE_BUILDING, DMA);

        u16 buildingAttr = TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, TILE_BUILDING);

        for (u16 x = 0; x < PLANE_WIDTH_TILES; x += 4)
            VDP_fillTileMapRect(BG_B, buildingAttr, x, 20, 1, PLANE_HEIGHT_TILES - 20);
    }

    VDP_setHorizontalScroll(BG_B, 0);
}

void Scroll_update(u8 speed)
{
    scrollX -= speed; // background drifts left as the ship advances right
    VDP_setHorizontalScroll(BG_B, scrollX);
}
