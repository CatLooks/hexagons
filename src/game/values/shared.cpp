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
	
	/// Returns offset of a tile level.
	sf::Vector2i tileLevel(sf::Vector2i size) {
		return { 0, size.y / 8 };
	};

	/// Size of icon texture.
	const sf::Vector2i iconTex = { 16, 16 };
	/// Size of rendered icon.
	const sf::Vector2i iconSize = iconTex * k;

	/// Width of button borders.
	const float buttonBorder = 4.f * k;

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
	/// Returns a tile in 16x16 texture map with a 64x64 offset.
	sf::IntRect coords(int x, int y, int dx, int dy) {
		return { { x * tileTex.x + dx * iconTex.x, y * tileTex.y + dy * iconTex.y }, iconTex };
	};

	/// Dark shade mask color.
	const sf::Color dimTint = sf::Color(0, 0, 0, 128);

	/// Missing texture replacement.
	const sf::IntRect missing = coords(9, 3);

	/// Keyboard pan speed multiplier.
	const float panSpeed = 240.f;
};