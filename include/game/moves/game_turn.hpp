#pragma once

// include dependencies
#include "move.hpp"

namespace Moves {
	/// Entity change move.
	///
	/// Stores a state change for a single entity.
	struct EntityChange : Move {
		EntState  state; /// New entity state (manually set).
		EntState a_prev; /// Previous entity state.

		/// Constructs an entity change move.
		///
		/// @param prev Previous entity state.
		EntityChange(EntState prev);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;
	};

	/// Region change move.
	///
	/// Stores a state change for a single region.
	struct RegionChange : Move {
		Region  state; /// New region state (manually set).
		Region a_prev; /// Previous region state.

		/// Constructs a region change move.
		/// 
		/// @param pos Region access state.
		/// @param prev Previous region state.
		RegionChange(sf::Vector2i pos, Region prev);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;
	};
};