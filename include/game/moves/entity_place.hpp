#pragma once

// include dependencies
#include "move.hpp"
#include "game/skill.hpp"

namespace Moves {
	/// Returns placement cost of an entity.
	/// 
	/// @param entity Placed entity.
	/// @param state Current skill selection state.
	int entity_cost(const Entity* entity, const SkillState& state);

	/// Entity placement move.
	struct EntityPlace : Move {
		EntState  entity; /// Placed entity.
		sf::Vector2i pos; /// Entity placement position.
		SkillState state; /// Skill state.

		/// Constructs an entity placement move.
		/// 
		/// @param entity Placed entity.
		/// @param pos Entity position.
		/// @param state Current skill selection state.
		EntityPlace(EntState entity, sf::Vector2i pos, SkillState state);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;

		/// Emits move section info.
		void emitDev(dev::Section* section, ui::Text::List& list) const override;
	};
};