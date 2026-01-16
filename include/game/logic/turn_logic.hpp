#pragma once

// include dependencies
#include "game/map.hpp"
#include "game/moves/game_turn.hpp"

namespace logic {
	/// Executes global turn transition logic.
	///
	/// @param map Map reference.
	History::UniqList global(Map* map);

	/// Executes turn transition logic for a team.
	/// 
	/// @param map Map reference.
	/// @param team Player team.
	History::UniqList turn(Map* map, Region::Team team);
};