#pragma once

// include dependencies
#include "dev_panel.hpp"

class Game;

namespace dev {
	/// Constructs a developer panel for the game.
	/// 
	/// @param game Game element.
	/// 
	/// @return Panel element.
	Panel* game_panel(Game* game);
};