#pragma once

// include dependencies
#include "skill_array.hpp"
#include "game/build.hpp"
#include "game/troop.hpp"

namespace logic {
	/// Building hitpoint count.
	/// @param id Building type.
	extern const int build_hp[Build::Count];

	/// Building base buying cost.
	/// @param id Building type.
	extern const int build_cost_base[Build::Count];

	/// Building buying cost.
	/// 
	/// @param type Building type.
	/// @param var Region variable counters.
	/// 
	/// @return True building cost.
	int build_cost(Build::Type type, const RegionVar& var);

	/// Extra farm cost per farm built.
	extern const int farm_cost_inc;
	/// Extra tent cost per tent built.
	extern const int tent_cost_inc;

	/// HP healed by tents.
	extern const int heal_amount[2];
	/// Tent heal range.
	extern const size_t heal_range;

	/// Beacon stun skill cost.
	extern const int stun_cost;
	/// Beacon stun skill range.
	extern const size_t stun_range;

	/// Building destruction bonus.
	/// @param id Building type.
	extern const int build_bonus[Build::Count];

	/// Building upkeep cost.
	/// @param id Building type.
	extern const int build_upkeep[Build::Count];

	/// Weakest troop that can damage the building.
	/// @param id Building type.
	extern const Troop::Type build_level[Build::Count];

	/// Weakest troop that can be in range of the building.
	/// @param id Building type.
	extern const Troop::Type build_guard[Build::Count];

	/// Building skills.
	extern const SkillArray build_skills[Build::Count];
};