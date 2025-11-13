#pragma once

// include dependencies
#include "game/values/hex_values.hpp"
#include "game/map.hpp"
#include "troop_draw.hpp"
#include "build_draw.hpp"
#include "plant_draw.hpp"
#include <functional>

namespace Draw {
	/// Tile drawing data.
	struct Tile {
		/// Neighbor test function signature.
		///
		/// @param origin Hex at origin.
		/// @param neighbor Neighbor of `origin`.
		///
		/// @return Whether the test passed.
		using NeighborTest = std::function<bool(const Hex* origin, const Hex* neighbor)>;

		const Map& map;      /// Map reference.
		const Hex* hex;      /// Hex reference.
		sf::Vector2i coords; /// Hex coordinates.
		sf::Vector2i origin; /// Draw origin position.

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
		/// @param test Border draw test.
		/// @param color Border color.
		void drawBorders(ui::RenderBuffer& target, NeighborTest test, sf::Color color) const;
	};

	/// Checks whether a neighbor is from a different region.
	///
	/// @param origin Hex at origin.
	/// @param neighbor Neighbor of `origin`.
	bool regionBorderTest(const Hex* origin, const Hex* neighbor);
};