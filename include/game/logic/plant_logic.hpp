#pragma once

// include dependencies
#include "skill_array.hpp"
#include "game/plant.hpp"

namespace logic {
	/// Plant cut down bonus.
	/// @param id Plant type.
	extern const int plant_bonus[Plant::Count];

	/// Plant upkeep cost.
	extern const int plant_upkeep;

	/// Plant skill.
	/// Applied for every plant.
	extern const SkillArray plant_skill;
};