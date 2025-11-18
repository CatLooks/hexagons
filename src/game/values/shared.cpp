#include "game/values/shared.hpp"

namespace Values {
	/// Texture to render space multiplier.
	const int k = 2;
	/// Base unit for tile related sizes.
	const int tileUnit = 64 * k;
	/// Size of tile texture.
	const sf::Vector2i tileTex = { 64, 64 };
	/// Size of rendered tile.
	const sf::Vector2i tileSize = { tileUnit, tileUnit };
	/// Offsets between tiles on map.
	const sf::Vector2i tileOff = { tileUnit, tileUnit * 3 / 4 };
	/// Offset of a tile level.
	const sf::Vector2i tileLevel = { 0, tileUnit / 8 };
	/// Thickness of map backplane border.
	const sf::Vector2i mapBorder = { tileUnit / 2, tileUnit / 2 };

	/// Returns horizontal offset for tile row.
	sf::Vector2i rowOffset(int y) {
		return { (y & 1) ? 0 : tileOff.x / 2, 0 };
	};

	/// Returns a 64x64 texture map.
	sf::IntRect coords(int x, int y) {
		return { { x * tileTex.x, y * tileTex.y }, tileTex };
	};

	/// Missing texture replacement.
	const sf::IntRect missing = coords(9, 3);
};