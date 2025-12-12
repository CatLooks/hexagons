#pragma once

// include dependencies
#include "game/values/entity_values.hpp"
#include "hex_draw.hpp"

namespace Draw {
	/// Entity health bar data.
	struct Bar {
		int hp;      /// Current HP.
		int max;     /// Maximum HP.
		bool poison; /// Whether the entity is poisoned.

		/// Applied effects.
		Entity::Effects effects;

		/// Constructs a health bar object.
		/// 
		/// @param entity Target entity.
		Bar(const Entity& entity);

		/// Checks whether the health bar is fully filled.
		bool filled() const;

		/// Draws health bar for an entity on the map.
		/// 
		/// @param pos Entity tile position.
		/// @param buffer Target render buffer.
		void drawAbove(sf::Vector2i pos, ui::RenderBuffer& buffer) const;

		/// Draws health bar for a preview entity.
		/// 
		/// @param area Draw area.
		/// @param buffer Target render buffer.
		/// @param team Region team.
		void drawSquare(sf::IntRect area, ui::RenderBuffer& buffer) const;
	};

	/// Draws entity health bar.
	/// 
	/// @param tile Tile draw data.
	/// @param target Target render buffer.
	void entityBar(const Tile& tile, ui::RenderBuffer& buffer);
};