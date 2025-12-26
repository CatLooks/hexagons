#include "game/logic/skill_list.hpp"

namespace SkillList {
	/// ======== SPEARMAN BLOCK EFFECT ======== ///
	const Skill effect_defend = {
		.type = Skills::Shield,
		.annotation = Skill::None,
		.resource = Skills::Peach,
		.cost = logic::troop_effect_cost[Troop::Spearman],
		.action = [](const SkillState&, Map& map, const HexRef& tile, const HexRef& _) -> Move* {
			// ignore if no entity
			if (!tile.hex->entity()) return nullptr;

			// create effect move
			return new Moves::EntityEffect(
				tile.pos, EffectType::Shielded,
				logic::troop_effect_cost[Troop::Knight]
			);
		},
		.format = Skill::Self
	};

	/// ======== ARCHER RANGE BOOST EFFECT ======== ///
	const Skill effect_range = {
		.type = Skills::RangeBoost,
		.annotation = Skill::None,
		.resource = Skills::Peach,
		.cost = logic::troop_effect_cost[Troop::Archer],
		.action = [](const SkillState&, Map& map, const HexRef& tile, const HexRef& _) -> Move* {
			// ignore if no entity
			if (!tile.hex->entity()) return nullptr;

			// create effect move
			return new Moves::EntityEffect(
				tile.pos, EffectType::RangeBoost,
				logic::troop_effect_cost[Troop::Archer]
			);
		},
		.format = Skill::Self
	};

	/// ======== BARON DEFENSE BOOST EFFECT ======== ///
	const Skill effect_defense = {
		.type = Skills::DefenseBoost,
		.annotation = Skill::None,
		.resource = Skills::Peach,
		.cost = logic::troop_effect_cost[Troop::Baron],
		.action = [](const SkillState&, Map& map, const HexRef& tile, const HexRef& _) -> Move* {
			// ignore if no entity
			if (!tile.hex->entity()) return nullptr;

			// create effect move
			return new Moves::EntityEffect(
				tile.pos, EffectType::DefenseBoost,
				logic::troop_effect_cost[Troop::Baron]
			);
		},
		.format = Skill::Self
	};

	/// ======== KNIGHT OFFENSE BOOST EFFECT ======== ///
	const Skill effect_offense = {
		.type = Skills::OffenseBoost,
		.annotation = Skill::None,
		.resource = Skills::Peach,
		.cost = logic::troop_effect_cost[Troop::Knight],
		.action = [](const SkillState&, Map& map, const HexRef& tile, const HexRef& _) -> Move* {
			// ignore if no entity
			if (!tile.hex->entity()) return nullptr;

			// create effect move
			return new Moves::EntityEffect(
				tile.pos, EffectType::OffenseBoost,
				logic::troop_effect_cost[Troop::Knight]
			);
		},
		.format = Skill::Self
	};
};