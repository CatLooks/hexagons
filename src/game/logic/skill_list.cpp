#include "game/logic/skill_list.hpp"

namespace SkillList {
	// entity buy
	const Skill buy_troop = {
		.type = Skills::Empty,
		.annotation = Skill::None,
		.action = [](Map& map, const HexRef& _, const HexRef& tile) {
			Troop troop;
			troop.pos = tile.pos;
			troop.type = Troop::Farmer;
			troop.hp = 1;
			map.setTroop(troop);
		},
		.format = Skill::Self
	};
	const Skill buy_build = { Skills::Empty, Skill::None };
	const Skill buy_troop_aim = {
		.type = Skills::Empty,
		.annotation = Skill::Aim,
		.select = [](const HexRef& tile, size_t idx) {
			return Spread {
				.hop = skillf::sameRegionHop(tile.hex->region),
				.pass = skillf::emptyPass,
				.effect = skillf::selectTile(idx),
				.imm = true
			};
		},
		.radius = ~0ull,
		.action = [](Map& map, const HexRef& _, const HexRef& tile) {
			Troop troop;
			troop.pos = tile.pos;
			troop.type = Troop::Farmer;
			troop.hp = 1;
			map.setTroop(troop);
		},
		.format = Skill::SingleAim
	};
	const Skill buy_build_aim = { Skills::Empty, Skill::Aim };

	// general troop / building skills
	const Skill withdraw = {
		.type = Skills::Withdraw,
		.annotation = Skill::None,
		.action = [](Map& map, const HexRef& tile, const HexRef& _) {
			map.removeEntity(tile.hex);
		},
		.format = Skill::Self
	};
	const Skill move = {
		.type = Skills::Move,
		.annotation = Skill::Aim,
		.select = [](const HexRef& tile, size_t idx) {
			return Spread {
				.hop = skillf::solidHop,
				.pass = skillf::emptyPass,
				.effect = skillf::selectTile(idx)
			};
		},
		.radius = 2,
		.action = [](Map& map, const HexRef& prev, const HexRef& next) {
			map.moveTroop(prev, next);
		},
		.format = Skill::SingleAim,
		.reselect = true
	};

	// attack skills
	const Skill attack_lumber = { Skills::AttackLumber, Skill::Aim };
	const Skill attack_spear  = { Skills::AttackSpear , Skill::Aim };
	const Skill attack_archer = { Skills::AttackArcher, Skill::Aim };
	const Skill attack_baron  = { Skills::AttackBaron , Skill::Aim };
	const Skill attack_knight = {
		.type = Skills::AttackKnight,
		.annotation = Skill::Aim,
		.select = [](const HexRef& tile, size_t idx) {
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
		.action = [](Map& map, const HexRef& prev, const HexRef& next) {
			next.hex->troop->hp--;
		},
		.format = Skill::SingleAim
	};

	// status effect skills
	const Skill effect_defend  = { Skills::Shield      , Skill::None };
	const Skill effect_range   = { Skills::RangeBoost  , Skill::None };
	const Skill effect_defense = { Skills::DefenseBoost, Skill::None };
	const Skill effect_offense = {
		.type = Skills::OffenseBoost,
		.annotation = Skill::None,
		.action = [](Map& map, const HexRef& tile, const HexRef& _) {
			map.effectTroop(tile, EffectType::OffenseBoost);
		},
		.format = Skill::Self
	};

	// other special skills
	const Skill fruit = { Skills::Harvest, Skill::Aim  };
	const Skill cut   = { Skills::TreeCut, Skill::Aim  };
	const Skill heal  = { Skills::Heal   , Skill::Aim  };
	const Skill stun  = { Skills::Stun   , Skill::None };
};