#pragma once

// include dependencies
#include "move.hpp"
#include "game/troop.hpp"
#include "game/build.hpp"
#include "game/plant.hpp"
#include "game/region.hpp"
#include <variant>

namespace Moves {
	/// Troop movement move.
	struct TroopMove : Move {
		sf::Vector2i dest; /// Move destination.

		/// Previous hex state.
		///
		/// This gets fetched when applying a move.
		struct Prev {
			/// Previous hex team.
			Region::Team team;
			/// Previous hex entity.
			EntState entity;
		} a_state;

		/// Constructs troop movement move.
		/// 
		/// @param dest Movement destination.
		TroopMove(sf::Vector2i dest);

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