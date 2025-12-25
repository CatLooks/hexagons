#pragma once

// include dependencies
#include "skill_array.hpp"
#include "game/build.hpp"
#include "game/troop.hpp"

namespace logic {
	/// Building buying cost.
	/// @param id Building type.
	extern const int build_cost[Build::Count];

	/// Building destruction bonus.
	/// @param id Building type.
	extern const int build_bonus[Build::Count];

	/// Building upkeep cost.
	/// @param id Building type.
	extern const int build_upkeep[Build::Count];

	/// Weakest troop that can damage the building.
	/// @param id Building type.
	extern const Troop::Type build_level[Build::Count];

	/// Building skills.
	extern const SkillArray build_skills[Build::Count];
};