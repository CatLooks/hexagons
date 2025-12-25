#pragma once

// include dependencies
#include "move.hpp"
#include "game/entity.hpp"

namespace Moves {
	/// Applies effect to an entity.
	struct EntityEffect : Move {
		sf::Vector2i  pos; /// Entity position.
		EffectType effect; /// Applied effect.
		bool       before; /// Whether the entity had the effect before.

		/// Constructs entity effect move.
		/// 
		/// @param pos Entity position.
		/// @param effect Applied effect.
		/// @param before Whether the entity had the effect before.
		EntityEffect(sf::Vector2i pos, EffectType effect, bool before);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;
	};
};