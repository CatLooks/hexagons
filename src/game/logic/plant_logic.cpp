#include "game/logic/plant_logic.hpp"

namespace logic {
	/// Plant cut down bonus.
	const int plant_bonus[Plant::Count] = { 2, 3, 3, 4, 6, 4, 0 };

	/// Plant upkeep cost.
	const int plant_upkeep = 1;

	/// Amount of berries needed to heal 1 HP.
	const int berry_per_hp = 3;

	// plant skill
	const SkillArray plant_skill = { SkillArray::L00, {} };
};