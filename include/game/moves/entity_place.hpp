#pragma once

// include dependencies
#include "move.hpp"
#include "game/skill.hpp"

namespace Moves {
	/// Returns placement cost of an entity.
	/// 
	/// @param entity Placed entity.
	/// @param var Region variable counters.
	int entity_cost(const Entity* entity, const RegionVar& var);

	/// Entity placement move.
	struct EntityPlace : Move {
		EntState entity; /// Placed entity.
		RegionVar   var; /// Region variable counters.

		/// Constructs an entity placement move.
		/// 
		/// @param entity Placed entity.
		/// @param var Region variable counters.
		EntityPlace(EntState entity, const RegionVar& var);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;

		/// Emits move section info.
		void emitDev(dev::Section* section, ui::Text::List& list) const override;
	};
};