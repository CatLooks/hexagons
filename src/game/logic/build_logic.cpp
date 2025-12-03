#include "game/logic/build_logic.hpp"

namespace logic {
	/// Building buying cost.
	const int build_cost[Build::Count] = { 0, 12, 80, 15, 35, 95 };

	/// Building destruction bonus.
	const int build_bonus[Build::Count] = { 30, 5, 8, 6, 16, 43 };

	/// Building upkeep cost.
	const int build_upkeep[Build::Count] = { 0, -4, 0, 1, 6, 29 };

	/// Weakest troop that can damage the building.
	const Troop::Type build_level[Build::Count] = {
		Troop::Spearman, Troop::Lumberjack, Troop::Spearman,
		Troop::Archer, Troop::Baron, Troop::Knight
	};
};