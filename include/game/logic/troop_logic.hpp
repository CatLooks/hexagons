#pragma once

// include dependencies
#include "skill_array.hpp"
#include "game/troop.hpp"

namespace logic {
	/// Troop max health.
	/// @param id Troop type.
	extern const int troop_hp[Troop::Count];

	/// Base troop damage.
	/// @param id Troop type.
	extern const int troop_dmg[Troop::Count];

	/// Troop power level.
	/// @param id Troop type.
	extern const int troop_pow[Troop::Count];

	/// Troop buying cost.
	/// @param id Troop type.
	extern const int troop_cost[Troop::Count];

	/// Troop upkeep cost.
	/// @param id Troop type.
	extern const int troop_upkeep[Troop::Count];

	/// Troop skills.
	extern const SkillArray troop_skills[Troop::Count];

	/// Cost to execute troop's special skill (in peaches).
	extern const int troop_effect_cost[Troop::Count];
};