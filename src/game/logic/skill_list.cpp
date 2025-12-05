#include "game/logic/skill_list.hpp"

namespace SkillList {
	// entity buy
	const SkillDesc buy_troop = {
		.type = Skill::Empty,
		.annotation = SkillDesc::None,
		.action = [](Map& map, const HexRef& _, const HexRef& tile) {
			Troop troop;
			troop.pos = tile.pos;
			troop.type = Troop::Farmer;
			troop.hp = 1;
			map.setTroop(troop);
		},
		.format = SkillDesc::Self
	};
	const SkillDesc buy_build = { Skill::Empty, SkillDesc::None };
	const SkillDesc buy_troop_aim = {
		.type = Skill::Empty,
		.annotation = SkillDesc::Aim,
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
		.format = SkillDesc::SingleAim
	};
	const SkillDesc buy_build_aim = { Skill::Empty, SkillDesc::Aim };

	// general troop / building skills
	const SkillDesc withdraw = {
		.type = Skill::Withdraw,
		.annotation = SkillDesc::None,
		.action = [](Map& map, const HexRef& tile, const HexRef& _) {
			map.removeEntity(tile.hex);
		},
		.format = SkillDesc::Self
	};
	const SkillDesc move = {
		.type = Skill::Move,
		.annotation = SkillDesc::Aim,
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
		.format = SkillDesc::SingleAim,
		.reselect = true
	};

	// attack skills
	const SkillDesc attack_lumber = { Skill::AttackLumber, SkillDesc::Aim };
	const SkillDesc attack_spear  = { Skill::AttackSpear , SkillDesc::Aim };
	const SkillDesc attack_archer = { Skill::AttackArcher, SkillDesc::Aim };
	const SkillDesc attack_baron  = { Skill::AttackBaron , SkillDesc::Aim };
	const SkillDesc attack_knight = {
		.type = Skill::AttackKnight,
		.annotation = SkillDesc::Aim,
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
		.format = SkillDesc::SingleAim
	};

	// status effect skills
	const SkillDesc effect_defend  = { Skill::Shield      , SkillDesc::None };
	const SkillDesc effect_range   = { Skill::RangeBoost  , SkillDesc::None };
	const SkillDesc effect_defense = { Skill::DefenseBoost, SkillDesc::None };
	const SkillDesc effect_offense = {
		.type = Skill::OffenseBoost,
		.annotation = SkillDesc::None,
		.action = [](Map& map, const HexRef& tile, const HexRef& _) {
			map.effectTroop(tile, EffectType::OffenseBoost);
		},
		.format = SkillDesc::Self
	};

	// other special skills
	const SkillDesc fruit = { Skill::Harvest, SkillDesc::Aim  };
	const SkillDesc cut   = { Skill::TreeCut, SkillDesc::Aim  };
	const SkillDesc heal  = { Skill::Heal   , SkillDesc::Aim  };
	const SkillDesc stun  = { Skill::Stun   , SkillDesc::None };
};