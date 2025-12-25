#include "game/logic/troop_logic.hpp"
#include "game/logic/skill_list.hpp"

namespace logic {
	/// Troop max health.
	const int troop_hp[Troop::Count] = { 1, 2, 3, 2, 4, 6 };

	/// Base troop damage.
	const int troop_dmg_base[Troop::Count] = { 0, 1, 2, 1, 3, 4 };

	/// Troop buying cost.
	const int troop_cost[Troop::Count] = { 10, 15, 20, 25, 30, 40 };

	/// Troop upkeep cost.
	const int troop_upkeep[Troop::Count] = { 2, 4, 6, 14, 18, 36 };

	/// Returns troop damage.
	int troop_dmg(const Troop& troop) {
		return troop_dmg_base[troop.type];
	};

	/// Returns troop defense.
	int troop_def(const Troop& troop) {
		return troop.hp;
	};

	/// Returns troop range.
	int troop_rng(const Troop& troop) {
		return troop.type == Troop::Archer ? 3 : 1;
	};

	// skill array
	const SkillArray troop_skills[Troop::Count] = {
		/* farmer */ { SkillArray::L12, {
			&SkillList::move,
			&SkillList::fruit,
			&SkillList::withdraw
		} },
		/* lumber */ { SkillArray::L22, {
			&SkillList::move,
			&SkillList::attack_lumber,
			&SkillList::cut,
			&SkillList::withdraw
		} },
		/* spear  */ { SkillArray::L22, {
			&SkillList::move,
			&SkillList::attack_spear,
			&SkillList::effect_defend,
			&SkillList::withdraw
		} },
		/* archer */ { SkillArray::L22, {
			&SkillList::move,
			&SkillList::attack_archer,
			&SkillList::effect_range,
			&SkillList::withdraw
		} },
		/* baron  */ { SkillArray::L22, {
			&SkillList::move,
			&SkillList::attack_baron,
			&SkillList::effect_defense,
			&SkillList::withdraw
		} },
		/* knight */ { SkillArray::L22, {
			&SkillList::move,
			&SkillList::attack_knight,
			&SkillList::effect_offense,
			&SkillList::withdraw
		} },
	};
};