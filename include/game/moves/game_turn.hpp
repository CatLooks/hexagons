#pragma once

// include dependencies
#include "move.hpp"

namespace Moves {
	/// Game turn move.
	///
	/// Stores a state change for a single entity.
	struct GameTurn : Move {
		EntState  state; /// New entity state.
		EntState a_prev; /// Previous entity state.

		/// Constructs a game turn move.
		///
		/// @param prev Previous entity state.
		GameTurn(EntState prev);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;

		/// Emits move section info.
		void emitDev(dev::Section* section, ui::Text::List& list) const override;
	};
};