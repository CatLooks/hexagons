#pragma once

// include dependencies
#include "game/values/troop_values.hpp"
#include "hex_draw.hpp"

namespace Draw {
	/// Draws a troop.
	///
	/// @param tile Tile draw data.
	/// @param target Target render buffer.
	void troopEntity(const Tile& tile, ui::RenderBuffer& buffer);

	/// Troop health bar data.
	struct HealthBar {
		int hp;      /// Current HP.
		int max;     /// Maximum HP.
		bool poison; /// Is poisoned?

		/// Constructs a health bar object.
		/// 
		/// @param hp Current HP.
		/// @param max Maximum HP.
		/// @param poison Is poisoned?
		HealthBar(int hp, int max, bool poison = false);

		/// Draws health bar above an entity.
		/// 
		/// @param pos Entity tile position.
		/// @param buffer Target render buffer.
		void draw(sf::Vector2i pos, ui::RenderBuffer& buffer) const;
	};
};