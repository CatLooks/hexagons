#pragma once

// include dependencies
#include "draw/hex_draw.hpp"
#include "draw/troop_draw.hpp"
#include "draw/build_draw.hpp"
#include "draw/plant_draw.hpp"

/// Tile drawer object.
/// Prepares tile drawing data for each tile in an area.
class TileDrawer {
private:
	const Map*   _map; /// Map reference.

	sf::Vector2i _coords; /// Next hex coordinates.
	sf::Vector2i _draw;   /// Next hex draw position.
	sf::Vector2i _size;   /// Draw size.

	sf::Vector2i _org; /// Draw origin.
	sf::Vector2i _beg; /// Draw area top-left corner.
	sf::Vector2i _end; /// Draw area end coords.

	sf::Vector2i _select = {-1, -1}; /// Selected tile coordinates.
	int _elev = 0;                   /// Selected tile elevation.

public:
	/// Constructs a tile drawer.
	/// 
	/// @param map Map reference.
	/// @param area Draw area.
	/// @param origin Draw origin.
	/// @param size Draw size.
	TileDrawer(const Map* map, sf::IntRect area, sf::Vector2i origin, sf::Vector2i size);

	/// Selects a tile for elevation.
	/// 
	/// @param coords Selected tile coordinates.
	/// @param elevation Selected tile elevation.
	void select(sf::Vector2i coords, int elevation);

	/// @return Selected tile coordinates.
	sf::Vector2i selected() const;

	/// Checks whether the tile is selected.
	/// 
	/// @param tile Tile object.
	bool selected(const Draw::Tile& tile) const;

	/// Resets the drawer to first tile.
	void reset();

	/// Returns the next tile drawing data.
	std::optional<Draw::Tile> next();
};