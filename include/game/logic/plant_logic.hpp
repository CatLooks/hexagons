#pragma once

// include dependencies
#include "skill_array.hpp"
#include "game/plant.hpp"
#include <optional>

namespace logic {
	/// Plant cut down bonus.
	/// @param id Plant type.
	extern const int plant_bonus[Plant::Count];

	/// Plant harvesting range.
	extern const size_t harvest_range;

	/// Plant upkeep cost.
	extern const int plant_upkeep;

	/// Amount of berries needed to heal 1 HP.
	extern const int berry_per_hp;

	/// Plant skill.
	/// Applied for every plant.
	extern const SkillArray plant_skill;
};