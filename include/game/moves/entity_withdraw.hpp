#pragma once

// include dependencies
#include "move.hpp"

namespace Moves {
	/// Withdraws the entity.
	struct EntityWithdraw : Move {
		sf::Vector2i pos; /// Entity position.
		EntState a_state; /// Entity state.

		/// Constructs entity withdraw move.
		/// 
		/// @param pos Entity position.
		EntityWithdraw(sf::Vector2i pos);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;
	};
};