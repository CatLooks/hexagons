#include "game/logic/skill_list.hpp"

namespace SkillList {
	/// ======== SPEARMAN BLOCK EFFECT ======== ///
	const Skill effect_defend = {
		.type = Skills::Shield,
		.annotation = Skill::Peach,
		.resource = Skills::Peach,
		.cost = skillf::cost(logic::troop_effect_cost[Troop::Spearman]),
		.action = [](const SkillState& state, Map& map, const HexRef& tile, const HexRef& _) -> Move* {
			// ignore if no entity
			if (!tile.hex->entity()) return nullptr;

			// create effect move
			return new Moves::EntityEffect(
				tile.pos, EffectType::Shielded,
				effect_defend.cost(state)
			);
		},
		.format = Skill::Self,
		.cooldown = 2
	};

	/// ======== ARCHER RANGE BOOST EFFECT ======== ///
	const Skill effect_range = {
		.type = Skills::RangeBoost,
		.annotation = Skill::Peach,
		.resource = Skills::Peach,
		.cost = skillf::cost(logic::troop_effect_cost[Troop::Archer]),
		.action = [](const SkillState& state, Map& map, const HexRef& tile, const HexRef& _) -> Move* {
			// ignore if no entity
			if (!tile.hex->entity()) return nullptr;

			// create effect move
			return new Moves::EntityEffect(
				tile.pos, EffectType::RangeBoost,
				effect_range.cost(state)
			);
		},
		.format = Skill::Self,
		.cooldown = 3
	};

	/// ======== BARON DEFENSE BOOST EFFECT ======== ///
	const Skill effect_defense = {
		.type = Skills::DefenseBoost,
		.annotation = Skill::Peach,
		.resource = Skills::Peach,
		.cost = skillf::cost(logic::troop_effect_cost[Troop::Baron]),
		.action = [](const SkillState& state, Map& map, const HexRef& tile, const HexRef& _) -> Move* {
			// ignore if no entity
			if (!tile.hex->entity()) return nullptr;

			// create effect move
			return new Moves::EntityEffect(
				tile.pos, EffectType::DefenseBoost,
				effect_defense.cost(state)
			);
		},
		.format = Skill::Self,
		.cooldown = 4
	};

	/// ======== KNIGHT OFFENSE BOOST EFFECT ======== ///
	const Skill effect_offense = {
		.type = Skills::OffenseBoost,
		.annotation = Skill::Peach,
		.resource = Skills::Peach,
		.cost = skillf::cost(logic::troop_effect_cost[Troop::Knight]),
		.action = [](const SkillState& state, Map& map, const HexRef& tile, const HexRef& _) -> Move* {
			// ignore if no entity
			if (!tile.hex->entity()) return nullptr;

			// create effect move
			return new Moves::EntityEffect(
				tile.pos, EffectType::OffenseBoost,
				effect_offense.cost(state)
			);
		},
		.format = Skill::Self,
		.cooldown = 4
	};
};