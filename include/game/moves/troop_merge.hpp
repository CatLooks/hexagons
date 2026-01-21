#pragma once

// include dependencies
#include "move.hpp"

namespace Moves {
	/// Troop merge move.
	struct TroopMerge : Move {
		sf::Vector2i dest; /// Merge position.
		Troop a_prev[2];   /// Previous troop states.

		/// Constructs a troop merge move.
		/// 
		/// @param dest Troop merge position.
		TroopMerge(sf::Vector2i dest);

		/// Applies the move.
		void onApply(Map* map) override;

		/// Reverts the move.
		void onRevert(Map* map) override;

		/// Returns tile to select after moving.
		std::optional<sf::Vector2i> applyCursor() override;

		/// Returns tile to select after returning.
		std::optional<sf::Vector2i> revertCursor() override;

		/// Emits move section info.
		void emitDev(dev::Section* section, ui::Text::List& list) const override;
	};
};