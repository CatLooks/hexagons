#pragma once

// include dependencies
#include "skill_array.hpp"
#include "game/build.hpp"
#include "game/troop.hpp"

namespace logic {
	/// Building buying cost.
	/// @param id Building type.
	extern const int build_cost[Build::Count];

	/// Extra farm cost per farm built.
	extern const int farm_cost_inc;

	/// HP healed by tents.
	extern const int heal_amount[2];
	/// Tent heal range.
	extern const int heal_range;

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