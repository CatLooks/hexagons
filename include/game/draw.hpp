#pragma once

// include dependencies
#include "map.hpp"
#include "values/textures.hpp"

// forward declare tile drawer
class TileDrawer;

namespace Draw {
	/// Tile drawing data.
	class Tile {
		friend TileDrawer;

	private:
		const Map& map;      /// Map reference.
		const Hex* hex;      /// Hex reference.
		sf::Vector2i coords; /// Hex coordinates.
		sf::Vector2i origin; /// Draw origin position.

	public:
		/// Constructs tile drawing data.
		/// 
		/// @param map Map reference.
		/// @param coords Hex coordinates.
		/// @param origin Draw origin position.
		Tile(const Map& map, sf::Vector2i coords, sf::Vector2i origin);

		/// Draws tile base.
		///
		/// @param target Target render buffer.
		void drawBase(ui::RenderBuffer& target) const;
		/// Draws tile borders.
		///
		/// @param target Target render buffer.
		/// @param color Border color.
		void drawBorders(ui::RenderBuffer& target, sf::Color color) const;
	};
};

/// Tile drawer object.
/// Prepares tile drawing data for each tile in an area.
class TileDrawer {
private:
	const Map&   _map; /// Map reference.

	sf::Vector2i _coords; /// Next hex coordinates.
	sf::Vector2i _draw;   /// Next hex draw position.

	sf::Vector2i _org; /// Draw origin.
	sf::Vector2i _beg; /// Draw area top-left corner.
	sf::Vector2i _end; /// Draw area end coords.

public:
	/// Constructs a tile drawer.
	/// 
	/// @param map Map reference.
	/// @param area Draw area.
	/// @param origin Draw origin.
	TileDrawer(const Map& map, sf::IntRect area, sf::Vector2i origin);

	/// Resets the drawer to first tile.
	void reset();

	/// Returns the next tile drawing data.
	std::optional<Draw::Tile> next();
};