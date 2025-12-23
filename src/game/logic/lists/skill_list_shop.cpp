#include "game/logic/skill_list.hpp"

namespace SkillList {
	/// ======== BUY TROOP IN-PLACE ======== ///
	const Skill buy_troop = {
		.type = Skills::Empty,
		.annotation = Skill::None,
		.action = [](const SkillState& state, Map& map, const HexRef& _, const HexRef& tile) {
			Troop troop;
			troop.pos = tile.pos;
			troop.type = Values::troop_shop[state.troop];
			troop.hp = troop.max_hp();
			map.setTroop(troop);
		},
		.format = Skill::Self,
		.cooldown = 0
	};

	/// ======== BUY TROOP ======== ///
	const Skill buy_troop_aim = {
		.type = Skills::Empty,
		.annotation = Skill::Aim,
		.select = [](const SkillState&, const HexRef& tile, size_t idx) {
			return Spread {
				.hop = skillf::sameRegionHop(tile.hex->region),
				.pass = skillf::emptyPass,
				.effect = skillf::selectTile(idx),
				.imm = true
			};
		},
		.radius = ~0ull,
		.action = [](const SkillState& state, Map& map, const HexRef& _, const HexRef& tile) {
			Troop troop;
			troop.pos = tile.pos;
			troop.type = Values::troop_shop[state.troop];
			troop.hp = troop.max_hp();
			map.setTroop(troop);
		},
		.format = Skill::SingleAim,
		.cooldown = 0
	};

	/// ======== BUY BUILDING IN-PLACE ======== ///
	const Skill buy_build = { Skills::Empty, Skill::None };

	/// ======== BUY BUILDING ======== ///
	const Skill buy_build_aim = { Skills::Empty, Skill::Aim };
};