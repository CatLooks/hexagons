#include "game/logic/build_logic.hpp"
#include "game/logic/skill_list.hpp"

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

	// skill array
	const SkillArray build_skills[Build::Count] = {
		/* castle */ { SkillArray::L10, {
			&SkillList::heal
		} },
		/* farm   */ { SkillArray::L01, {
			&SkillList::withdraw
		} },
		/* mine   */ { SkillArray::L01, {
			&SkillList::withdraw
		} },
		/* tower  */ { SkillArray::L01, {
			&SkillList::withdraw
		} },
		/* beacon */ { SkillArray::L11, {
			&SkillList::stun,
			&SkillList::withdraw
		} },
		/* fort   */ { SkillArray::L11, {
			&SkillList::stun,
			&SkillList::withdraw
		} }
	};
};