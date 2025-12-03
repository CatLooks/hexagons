#include "game/logic/skill_list.hpp"

namespace SkillList {
	// entity buy
	const SkillDesc buy_troop = { Skill::Empty, SkillDesc::None };
	const SkillDesc buy_build = { Skill::Empty, SkillDesc::None };
	const SkillDesc buy_troop_aim = { Skill::Empty, SkillDesc::Aim };
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
				.pass = [=](const Spread::Tile& tile) {
					return tile.hex->free();
				},
				.effect = skillf::selectTile(idx)
			};
		},
		.radius = 2,
		.action = [](Map& map, const HexRef& prev, const HexRef& next) {
			map.moveTroop(prev, next);
		},
		.format = SkillDesc::SingleAim
	};

	// attack skills
	const SkillDesc attack_lumber = { Skill::AttackLumber, SkillDesc::Aim };
	const SkillDesc attack_spear  = { Skill::AttackSpear , SkillDesc::Aim };
	const SkillDesc attack_archer = { Skill::AttackArcher, SkillDesc::Aim };
	const SkillDesc attack_baron  = { Skill::AttackBaron , SkillDesc::Aim };
	const SkillDesc attack_knight = { Skill::AttackKnight, SkillDesc::Aim };

	// status effect skills
	const SkillDesc effect_defend  = { Skill::Shield      , SkillDesc::None };
	const SkillDesc effect_range   = { Skill::RangeBoost  , SkillDesc::None };
	const SkillDesc effect_defense = { Skill::DefenseBoost, SkillDesc::None };
	const SkillDesc effect_offense = {
		.type = Skill::OffenseBoost,
		.annotation = SkillDesc::None,
		.action = [](Map& map, const HexRef& tile, const HexRef& _) {
			printf("offense\n");
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