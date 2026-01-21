#include "game/logic/plant_logic.hpp"

namespace logic {
	/// Plant cut down bonus.
	const int plant_bonus[Plant::Count] = { 2, 3, 3, 4, 6, 4, 0 };

	/// Plant harvesting range.
	const size_t harvest_range = 2;

	/// Plant upkeep cost.
	const int plant_upkeep = 1;

	/// Amount of berries needed to heal 1 HP.
	const int berry_per_hp = 3;

	/// Next plant in plant growth stage.
	const Plant::Type plant_grow[Plant::Count] = {
		/* bush  */ Plant::Berry,
		/* berry */ {},
		/* sap   */ Plant::Tree,
		/* tree  */ Plant::Peach,
		/* peach */ {},
		/* pine  */ {},
		/* grave */ {},
	};

	/// Plant to place when spreading.
	const Plant::Type plant_spread[Plant::Count] = {
		/* bush  */ Plant::Bush,
		/* berry */ Plant::Bush,
		/* sap   */ {},
		/* tree  */ Plant::Sapling,
		/* peach */ Plant::Sapling,
		/* pine  */ Plant::Pine,
		/* grave */ {},
	};

	// plant skill
	const SkillArray plant_skill = { SkillArray::L00, {} };
};