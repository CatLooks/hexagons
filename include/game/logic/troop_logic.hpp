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
	extern const int troop_dmg_base[Troop::Count];

	/// Troop buying cost.
	/// @param id Troop type.
	extern const int troop_cost[Troop::Count];

	/// Troop upkeep cost.
	/// @param id Troop type.
	extern const int troop_upkeep[Troop::Count];

	/// Returns troop damage.
	///
	/// @param troop Troop object.
	int troop_dmg(const Troop& troop);

	/// Returns troop defense.
	///
	/// @param troop Troop object.
	int troop_def(const Troop& troop);

	/// Returns troop range.
	///
	/// @param troop Troop object.
	int troop_rng(const Troop& troop);

	/// Troop skills.
	extern const SkillArray troop_skills[Troop::Count];

	/// Cost to execute troop's special skill (in peaches).
	extern const int troop_effect_cost[Troop::Count];
};