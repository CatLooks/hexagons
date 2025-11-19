#pragma once

// include dependencies
#include "game/values/hex_values.hpp"
#include "game/map.hpp"
#include <functional>

namespace Draw {
	/// Returns a white or a black color, depending on the input.
	/// 
	/// @param enable `true` for white, `false` for black.
	sf::Color white(bool enable);

	/// Tile drawing data.
	struct Tile {
		/// Neighbor test function signature.
		///
		/// @param origin Hex at origin.
		/// @param neighbor Neighbor of `origin`.
		///
		/// @return Whether the test passed.
		using NeighborTest = std::function<bool(const Hex* origin, const Hex* neighbor)>;

		const Map* map;      /// Map reference.
		const Hex* hex;      /// Hex reference.
		sf::Vector2i coords; /// Hex coordinates.
		sf::Vector2i origin; /// Draw origin position.
		sf::Vector2i size;   /// Draw size.

		/// Constructs tile drawing data.
		/// 
		/// @param map Map reference.
		/// @param coords Hex coordinates.
		/// @param origin Draw origin position.
		/// @param size Draw size.
		Tile(const Map* map, sf::Vector2i coords, sf::Vector2i origin, sf::Vector2i size);

		/// Draws tile base.
		///
		/// @param target Target render buffer.
		void drawBase(ui::RenderBuffer& target) const;

		/// Draws tile borders.
		///
		/// @param target Target render buffer.
		/// @param select Selected tile position.
		/// @param color Border color.
		void drawBorders(ui::RenderBuffer& target, sf::Vector2i select, sf::Color color) const;

		/// Draws tile sides.
		///
		/// @param target Target render buffer.
		/// @param select Selected tile position.
		/// @param up Upper gradient color.
		/// @param low Lower gradient color.
		void drawSides(ui::RenderBuffer& target, sf::Vector2i select, sf::Color up, sf::Color low) const;
	};
};