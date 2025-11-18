#pragma once

// include dependencies
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

namespace Values {
	/// Texture to render space multiplier.
	extern const int k;
	/// Base unit for tile related sizes.
	extern const int tileUnit;
	/// Size of tile texture.
	extern const sf::Vector2i tileTex;
	/// Size of rendered tile.
	extern const sf::Vector2i tileSize;
	/// Offsets between tiles on map.
	extern const sf::Vector2i tileOff;
	/// Offset of a tile level.
	extern const sf::Vector2i tileLevel;
	/// Thickness of map backplane border.
	extern const sf::Vector2i mapBorder;

	/// Returns horizontal offset for tile row.
	///
	/// @param y Row Y coordinate.
	sf::Vector2i rowOffset(int y);

	/// Returns a tile in 64x64 texture map.
	sf::IntRect coords(int x, int y);

	/// Missing texture replacement.
	extern const sf::IntRect missing;
};