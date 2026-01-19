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
		/// @param color Border color.
		void drawBorders(ui::RenderBuffer& target, sf::Color color) const;

		/// Draws tile sides.
		///
		/// @param target Target render buffer.
		/// @param up Upper gradient color.
		/// @param low Lower gradient color.
		/// @param mode Side drawing mode.
		void drawSides(ui::RenderBuffer& target, sf::Color up, sf::Color low) const;

		/// Draws unselected tile shading.
		/// 
		/// @param target Target render buffer.
		void drawShade(ui::RenderBuffer& target) const;

		/// Draws debug stuff.
		/// 
		/// @param target Target render buffer.
		void drawDebug(ui::RenderBuffer& target) const;

		/// Draws all tile contents.
		/// 
		/// @param target Target render buffer.
		void drawContents(ui::RenderBuffer& target) const;

		/// Draws pulse around tile.
		///
		/// @param target Target render buffer.
		/// @param t Pulse progression.
		void drawPulse(ui::RenderBuffer& target, float t) const;

		/// Draws building defensive shield at a tile.
		/// 
		/// @param target Target render buffer.
		/// @param t Opacity animation progression.
		void drawShield(ui::RenderBuffer& target, float t) const;

		/// Draws void tile base mask.
		/// 
		/// @param target Target render buffer.
		void drawVoidBase(ui::RenderBuffer& target) const;

		/// Draws void tile side mask.
		/// 
		/// @param target Target render buffer.
		void drawVoidSides(ui::RenderBuffer& target) const;
	};
};