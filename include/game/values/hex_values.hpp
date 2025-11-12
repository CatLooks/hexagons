#pragma once
#include "shared.hpp"
#include "game/map.hpp"

namespace Values {
	/// Water hex.
	extern const sf::IntRect water;
	/// Hex sides.
	extern const sf::IntRect sides;

	/// Texture maps for teams.
	/// @param id Team ID.
	extern const sf::IntRect hex_textures[Hex::Count];

	/// Localization requests for teams.
	/// @param id Team ID.
	extern const char* hex_names[Hex::Count];
};

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