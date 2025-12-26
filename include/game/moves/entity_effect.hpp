#pragma once

// include dependencies
#include "move.hpp"
#include "game/entity.hpp"

namespace Moves {
	/// Applies effect to an entity.
	struct EntityEffect : Move {
		sf::Vector2i  pos; /// Entity position.
		EffectType effect; /// Applied effect.
		int         peach; /// Effect cost (in peaches).
		bool     a_before; /// Whether the entity had the effect before.

		/// Constructs entity effect move.
		/// 
		/// @param pos Entity position.
		/// @param effect Applied effect.
		/// @param peach Effect cost.
		EntityEffect(sf::Vector2i pos, EffectType effect, int peach);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;
	};
};