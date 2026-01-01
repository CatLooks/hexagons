#pragma once

// include dependencies
#include "move.hpp"

namespace Moves {
	/// Troop attack move.
	struct TroopAttack : Move {
		sf::Vector2i     dest; /// Attacked entity position.
		int             a_dmg; /// Dealt damage.
		EntState      a_state; /// Previous entity state.
		Entity::Effects a_eff; /// Previous attacker effects.

		/// Constructs a troop attack move.
		/// 
		/// @param dest Attack destination.
		TroopAttack(sf::Vector2i dest);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;

		/// Emits move section info.
		void emitDev(dev::Section* section, ui::Text::List& list) const override;
	};
};