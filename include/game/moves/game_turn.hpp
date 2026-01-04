#pragma once

// include dependencies
#include "move.hpp"

namespace Moves {
	/// Game turn move.
	struct GameTurn : Move {
		std::vector<EntState> states; /// List of all changed entity states.
		std::vector<EntState> a_prev; /// List of all previous entity states.

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;

		/// Emits move section info.
		void emitDev(dev::Section* section, ui::Text::List& list) const override;
	};
};