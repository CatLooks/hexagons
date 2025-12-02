#pragma once

// include dependencies
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

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

	/// Returns offset of a tile level.
	///
	/// @param size Tile size.
	sf::Vector2i tileLevel(sf::Vector2i size);

	/// Size of icon texture.
	extern const sf::Vector2i iconTex;
	/// Size of rendered icon.
	extern const sf::Vector2i iconSize;

	/// Width of button borders.
	extern const float buttonBorder;

	/// Thickness of map backplane border.
	extern const sf::Vector2i mapBorder;

	/// Returns horizontal offset for tile row.
	///
	/// @param y Row Y coordinate.
	sf::Vector2i rowOffset(int y);

	/// Returns a tile in 64x64 texture map.
	sf::IntRect coords(int x, int y);
	/// Returns a tile in 16x16 texture map with a 64x64 offset.
	/// 
	/// @param x,y 64x64 coordinates.
	/// @param dx,dy 16x16 offset.
	sf::IntRect coords(int x, int y, int dx, int dy);

	/// Dark shade mask color.
	extern const sf::Color dimTint;

	/// Missing texture replacement.
	extern const sf::IntRect missing;

	/// Keyboard pan speed multiplier.
	extern const float panSpeed;
};