#pragma once

// include dependencies
#include "dev_panel.hpp"

class Game;

namespace dev {
	/// Developer panel factory.
	struct Factory {
		// no instances
		Factory() = delete;

		/// Constructs a developer panel for the game.
		/// 
		/// @param game Game element.
		/// 
		/// @return Panel element.
		static Panel* game_panel(Game* game);

		/// Creates sections for game state.
		/// 
		/// @param panel Panel element.
		/// @param game Game element.
		static void attach_state(Panel* panel, Game* game);

		/// Creates sections for map related stuff.
		/// 
		/// @param panel Panel element.
		/// @param game Game element.
		static void attach_map(Panel* panel, Game* game);

		/// Creates sections for all moves.
		///
		/// @param panel Panel element.
		/// @param game Game element.
		static void attach_moves(Panel* panel, Game* game);
	};
};