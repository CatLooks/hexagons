#include "game/logic/skill_list.hpp"

namespace SkillList {
	/// ======== SPEARMAN BLOCK EFFECT ======== ///
	const Skill effect_defend = { Skills::Shield, Skill::None };

	/// ======== ARCHER RANGE BOOST EFFECT ======== ///
	const Skill effect_range = { Skills::RangeBoost, Skill::None };

	/// ======== BARON DEFENSE BOOST EFFECT ======== ///
	const Skill effect_defense = { Skills::DefenseBoost, Skill::None };

	/// ======== KNIGHT OFFENSE BOOST EFFECT ======== ///
	const Skill effect_offense = {
		.type = Skills::OffenseBoost,
		.annotation = Skill::None,
		.resource = Skills::Peach,
		.cost = 10,
		.action = [](const SkillState&, Map& map, const HexRef& tile, const HexRef& _) {
			return map.effectTroop(tile, EffectType::OffenseBoost);
		},
		.format = Skill::Self
	};
};