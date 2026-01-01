#pragma once

// include dependencies
#include "move.hpp"

namespace Moves {
	/// Returns placement cost of an entity.
	/// 
	/// @param entity Placed entity.
	int entity_cost(const Entity* entity);

	/// Entity placement move.
	struct EntityPlace : Move {
		EntState  entity; /// Placed entity.
		sf::Vector2i pos; /// Entity placement position.

		/// Constructs an entity placement move.
		/// 
		/// @param entity Placed entity.
		/// @param pos Entity position.
		EntityPlace(EntState entity, sf::Vector2i pos);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;

		/// Emits move section info.
		void emitDev(dev::Section* section, ui::Text::List& list) const override;
	};
};