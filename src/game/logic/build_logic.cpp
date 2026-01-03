#include "game/logic/build_logic.hpp"
#include "game/logic/skill_list.hpp"

namespace logic {
	/// Building buying cost.
	const int build_cost_base[Build::Count] = { 0, 4, 80, 15, 35, 95, 10 };

	/// Building buying cost.
	int build_cost(Build::Type type, const SkillState& state) {
		int cost = build_cost_base[type];

		// increase farm cost
		if (type == Build::Farm && state.region)
			cost += farm_cost_inc * state.region->farms;

		// increase tent cost
		if (type == Build::Tent && state.region)
			cost += tent_cost_inc * state.region->tents;
		return cost;
	};

	/// Extra farm cost per farm built.
	const int farm_cost_inc = 1;
	/// Extra tent cost per tent built.
	const int tent_cost_inc = 2;

	/// HP healed by tents.
	const int heal_amount[2] = { 1, 2 };
	/// Tent heal range.
	const size_t heal_range = 1;

	/// Beacon stun skill cost.
	const int stun_cost = 20;
	/// Beacon stun skill range.
	const size_t stun_range = 3;

	/// Building destruction bonus.
	const int build_bonus[Build::Count] = { 30, 5, 8, 6, 16, 43, 8 };

	/// Building upkeep cost.
	const int build_upkeep[Build::Count] = { 0, -4, 0, 2, 8, 29, 1 };

	/// Weakest troop that can damage the building.
	const Troop::Type build_level[Build::Count] = {
		/* castle */ Troop::Spearman,
		/* farm   */ Troop::Lumberjack,
		/* mine   */ Troop::Spearman,
		/* tower  */ Troop::Archer,
		/* beacon */ Troop::Baron,
		/* fort   */ Troop::Knight,
		/* tent   */ Troop::Lumberjack
	};

	// skill array
	const SkillArray build_skills[Build::Count] = {
		/* castle */ { SkillArray::L00, {} },
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
		} },
		/* tent   */ { SkillArray::L21,
			&SkillList::heal1,
			&SkillList::heal2,
			&SkillList::withdraw
		}
	};
};