#include "game/logic/skill_list.hpp"

namespace SkillList {
	/// Attack action.
	static const Skill::Action troop_attack =
		[](const SkillState&, Map& map, const HexRef& prev, const HexRef& next) -> Move*
	{
		// ignore if no troop
		if (!prev.hex->troop || !next.hex->troop) return nullptr;

		// create attack move
		return new Moves::TroopAttack(next.pos);
	};

	/// ======== LUMBERJACK ATTACK ======== ///
	const Skill attack_lumber = {
		.type = Skills::AttackLumber,
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
		.action = troop_attack,
		.format = Skill::SingleAim
	};

	/// ======== SPEARMAN ATTACK ======== ///
	const Skill attack_spear = {
		.type = Skills::AttackSpear,
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
		.action = troop_attack,
		.format = Skill::SingleAim
	};

	/// ======== ARCHER ATTACK ======== ///
	const Skill attack_archer = {
		.type = Skills::AttackArcher,
		.annotation = Skill::Aim,
		.select = [](const SkillState&, const HexRef& tile, size_t idx) {
			return Spread {
				.pass = [&](const Spread::Tile& now) {
					return now.hex->team != tile.hex->team
						&& (bool)now.hex->troop;
				},
				.effect = skillf::selectTile(idx),
				.radius = [](const Spread::Tile& now) -> std::optional<size_t> {
					// range boosted attack
					if (now.hex->troop && now.hex->troop->hasEffect(EffectType::RangeBoost))
						return 3;
					return std::nullopt;
				}
			};
		},
		.radius = 2,
		.action = troop_attack,
		.format = Skill::SingleAim
	};

	/// ======== BARON ATTACK ======== ///
	const Skill attack_baron = {
		.type = Skills::AttackBaron,
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
		.action = troop_attack,
		.format = Skill::SingleAim
	};

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
		.action = troop_attack,
		.format = Skill::SingleAim
	};
};