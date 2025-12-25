#include "game/logic/skill_list.hpp"

namespace SkillList {
	/// ======== ENTITY WITHDRAW ======== ///
	const Skill withdraw = {
		.type = Skills::Withdraw,
		.annotation = Skill::None,
		.action = [](const SkillState&, Map& map, const HexRef& tile, const HexRef& _) {
			map.removeEntity(tile.hex);
			return nullptr;
		},
		.format = Skill::Self
	};

	/// ======== TROOP MOVE ======== ///
	const Skill move = {
		.type = Skills::Move,
		.annotation = Skill::Aim,
		.select = [](const SkillState&, const HexRef& tile, size_t idx) {
			return Spread {
				.hop = skillf::solidHop,
				.pass = skillf::emptyPass,
				.effect = skillf::selectTile(idx)
			};
		},
		.radius = 2,
		.action = [](const SkillState&, Map& map, const HexRef& prev, const HexRef& next) {
			map.moveTroop(prev, next);
			return nullptr;
		},
		.format = Skill::SingleAim,
		.reselect = true
	};

	/// ======== FRUIT HARVEST ======== ///
	const Skill fruit = { Skills::Harvest, Skill::Aim };

	/// ======== PLANT CUT-DOWN ======== ///
	const Skill cut = { Skills::TreeCut, Skill::Aim };

	/// ======== HEAL OTHER TROOPS ======== ///
	const Skill heal = { Skills::Heal, Skill::Aim };

	/// ======== BEACON STUN ======== ///
	const Skill stun = { Skills::Stun, Skill::None };
};