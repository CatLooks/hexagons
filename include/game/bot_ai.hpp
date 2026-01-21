#pragma once

// include dependencies
#include "template.hpp"

/// Bot AI related stuff.
namespace ai {
	/// Generates a list of moves for a team.
	/// 
	/// @param map Map reference.
	/// @param team Controlled team.
	/// @param diff AI difficulty.
	/// 
	/// @return Move list.
	History::UniqList generate(Map& map, Region::Team team, float diff);
};