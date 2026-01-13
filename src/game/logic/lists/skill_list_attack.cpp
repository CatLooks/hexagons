#include "game/logic/skill_list.hpp"

namespace SkillList {
	/// Attack target selection.
	static const Skill::Selection attack_select =
		[](const SkillState&, const HexRef& tile, size_t idx)
	{
		// create base spreader
		Spread spr = {
			.pass = [&](const Spread::Tile& now) {
				// ignore if the same team
				if (now.hex->team == tile.hex->team) return false;

				// ignore if no entity
				if (!(now.hex->troop || now.hex->build)) return false;

				// @todo add attack cancel if entity levels are too far apart
				// p.s. like build_level
				return true;
			},
			.effect = skillf::selectTile(idx)
		};

		// add solid hop to ground attacks
		if (!(tile.hex && tile.hex->troop && tile.hex->troop->type == Troop::Archer))
			spr.hop = skillf::solidHop;

		// add radius override for archer
		else spr.radius = [](const Spread::Tile& now) -> std::optional<size_t> {
			// range boosted attack
			if (now.hex->troop && now.hex->troop->hasEffect(EffectType::RangeBoost))
				return 3;
			return std::nullopt;
		};
		return spr;
	};

	/// Attack action.
	static const Skill::Action troop_attack =
		[](const SkillState&, Map& map, const HexRef& prev, const HexRef& next) -> Move*
	{
		// ignore if no troop and target
		if (!prev.hex->troop || !next.hex->entity()) return nullptr;

		// create attack move
		return new Moves::TroopAttack(next.pos);
	};

	/// ======== LUMBERJACK ATTACK ======== ///
	const Skill attack_lumber = {
		.type = Skills::AttackLumber,
		.annotation = Skill::Aim,
		.select = attack_select,
		.radius = 1,
		.action = troop_attack,
		.format = Skill::SingleAim
	};

	/// ======== SPEARMAN ATTACK ======== ///
	const Skill attack_spear = {
		.type = Skills::AttackSpear,
		.annotation = Skill::Aim,
		.select = attack_select,
		.radius = 1,
		.action = troop_attack,
		.format = Skill::SingleAim
	};

	/// ======== ARCHER ATTACK ======== ///
	const Skill attack_archer = {
		.type = Skills::AttackArcher,
		.annotation = Skill::Aim,
		.select = attack_select,
		.radius = 2,
		.action = troop_attack,
		.format = Skill::SingleAim
	};

	/// ======== BARON ATTACK ======== ///
	const Skill attack_baron = {
		.type = Skills::AttackBaron,
		.annotation = Skill::Aim,
		.select = attack_select,
		.radius = 1,
		.action = troop_attack,
		.format = Skill::SingleAim
	};

	/// ======== KNIGHT ATTACK ======== ///
	const Skill attack_knight = {
		.type = Skills::AttackKnight,
		.annotation = Skill::Aim,
		.select = attack_select,
		.radius = 1,
		.action = troop_attack,
		.format = Skill::SingleAim
	};
};