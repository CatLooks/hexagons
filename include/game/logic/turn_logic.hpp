#pragma once

// include dependencies
#include "game/map.hpp"
#include "game/moves/game_turn.hpp"

namespace logic {
	/// Executes turn transition logic.
	/// 
	/// @param map Map reference.
	History turn(Map* map);
};