#pragma once

// include dependencies
#include "move.hpp"

namespace Moves {
	/// Plant cut move.
	struct PlantCut : Move {
		sf::Vector2i pos; /// Plant position.
		Plant    a_state; /// Previous plant state.

		/// Constructs a plant cut move.
		/// 
		/// @param pos Plant position.
		PlantCut(sf::Vector2i pos);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;

		/// Emits move section info.
		void emitDev(dev::Section* section, ui::Text::List& list) const override;
	};
};