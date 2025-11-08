#pragma once

/// Base tile size.
/// Used to draw unscaled map.
#define TILE 64

/// Even row offset.
/// Every even row is shifted by this amount.
#define TILE_ROW_OFF (TILE / 2)

/// 1 tile horizontal offset.
#define TILE_X_OFF TILE
/// 1 tile vertical offset.
#define TILE_Y_OFF (TILE * 3 / 4)

/// Water tile downward shift.
#define TILE_WATER (TILE / 8)

/// Map backplane border size.
#define MAP_BORDER (TILE * 3 / 2)