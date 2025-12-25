#include "game/logic/skill_list.hpp"

namespace SkillList {
	/// ======== LUMBERJACK ATTACK ======== ///
	const Skill attack_lumber = { Skills::AttackLumber, Skill::Aim };

	/// ======== SPEARMAN ATTACK ======== ///
	const Skill attack_spear = { Skills::AttackSpear , Skill::Aim };

	/// ======== ARCHER ATTACK ======== ///
	const Skill attack_archer = { Skills::AttackArcher, Skill::Aim };

	/// ======== BARON ATTACK ======== ///
	const Skill attack_baron = { Skills::AttackBaron , Skill::Aim };

	/// ======== KNIGHT ATTACK ======== ///
	const Skill attack_knight = {
		.type = Skills::AttackKnight,
		.annotation = Skill::Aim,
		.select = [](const SkillState&, const HexRef& tile, size_t idx) {
			return Spread {
				.hop = skillf::solidHop,
				.pass = [&](const Spread::Tile& now) {
					return now.hex->team != tile.hex->team
						&& (bool)now.hex->troop;
				},
				.effect = skillf::selectTile(idx)
			};
		},
		.radius = 1,
		.action = [](const SkillState&, Map& map, const HexRef& prev, const HexRef& next) {
			next.hex->troop->hp--;
			return nullptr;
		},
		.format = Skill::SingleAim
	};
};