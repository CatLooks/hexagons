#pragma once

// include dependencies
#include "game/map.hpp"
#include "game/moves/game_turn.hpp"
#include "game/sync/messages.hpp"

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

	/// Team tile count.
	struct TileCount {
		/// Tile count for a single team.
		struct TeamInfo {
			Region::Team team = Region::Unclaimed;
			size_t tiles = 0;
		};

		/// List of team counters.
		std::vector<TeamInfo> teams;
		/// Total tile count.
		size_t total = 0;
	};

	/// Counts tiles for every team.
	/// 
	/// @param map Map reference.
	/// @param players Player list.
	/// 
	/// @return Map tile count.
	TileCount count(Map* map, const std::vector<Messages::Player>& players);

	/// Returns a victor of the game.
	/// 
	/// @param count Tile count object.
	/// 
	/// @return `Unclaimed` if no current victors.
	Region::Team win(const TileCount& count);
};