#include "game/logic/skill_list.hpp"

namespace SkillList {
	/// ======== ENTITY WITHDRAW ======== ///
	const Skill withdraw = {
		.type = Skills::Withdraw,
		.annotation = Skill::None,
		.action = [](const SkillState&, Map& map, const HexRef& tile, const HexRef& _) {
			// create withdraw move
			return new Moves::EntityWithdraw(tile.pos);
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
				.effect = skillf::selectTile(idx)
			};
		},
		.radius = 2,
		.action = [](const SkillState&, Map& map, const HexRef& prev, const HexRef& next) -> Move* {
			// ignore if no troop
			if (!prev.hex->troop) return nullptr;

			// create movement move
			return new Moves::TroopMove(next.pos);
		},
		.format = Skill::SingleAim,
		.reselect = true
	};

	/// ======== FRUIT HARVEST ======== ///
	const Skill fruit = { Skills::Harvest, Skill::Aim };

	/// ======== PLANT CUT-DOWN ======== ///
	const Skill cut = { Skills::TreeCut, Skill::Aim };

	/// ======== HEAL OTHER TROOPS ======== ///
	const Skill tent = {
		.type = Skills::TentSet,
		.annotation = Skill::Aim,
		.resource = Skills::Money,
		.cost = skillf::cost(logic::build_cost[Build::Tent]),
		.select = [](const SkillState&, const HexRef& tile, size_t idx) {
			return Spread {
				.hop = skillf::solidHop,
				.pass = skillf::sameRegionEmptyHop(tile.hex->region()),
				.effect = skillf::selectTile(idx)
			};
		},
		.radius = 1,
		.action = [](const SkillState&, Map& map, const HexRef& prev, const HexRef& next) -> Move* {
			// ignore if no troop
			if (!prev.hex->troop) return nullptr;

			// create tent entity
			Build build;
			build.type = Build::Tent;
			build.hp = build.max_hp();
			build.add_cooldown(Skills::Withdraw, 1);

			// create tent setup move
			return new Moves::EntityPlace(build, next.pos);
		},
		.format = Skill::SingleAim,
		.cooldown = 2,
		.reselect = true
	};

	/// ======== HEAL OTHER TROOPS ======== ///
	Skill heal1 = {
		.type = Skills::Heal_1,
		.annotation = Skill::Berry,
		.resource = Skills::Berry,
		.condition = [](const SkillState& state, Map& map, const HexRef& tile) {
			// get heal cost
			int cost = 0;
			auto spread = Moves::troopHealCost(logic::heal_amount[0], tile.hex->team, &cost);
			spread.apply(map, tile.pos, logic::heal_range);

			// store new heal cost
			heal1.cost = [=](const SkillState&) { return cost; };

			// check if enough resources are present
			return state.with(heal1.resource) >= cost;
		},
		.radius = (size_t)logic::heal_range,
		.action = [](const SkillState&, Map& map, const HexRef& _, const HexRef& tile) -> Move* {
			// ignore if no building
			if (!tile.hex->build) return nullptr;

			// create heal move
			return new Moves::TroopHeal(
				tile.pos, logic::heal_range,
				logic::heal_amount[0], tile.hex->team
			);
		},
		.format = Skill::Self,
		.cooldown = 2
	};
	Skill heal2 = {
		.type = Skills::Heal_2,
		.annotation = Skill::Berry,
		.resource = Skills::Berry,
		.condition = [](const SkillState& state, Map& map, const HexRef& tile) {
			// get heal cost
			int cost = 0;
			auto spread = Moves::troopHealCost(logic::heal_amount[1], tile.hex->team, &cost);
			spread.apply(map, tile.pos, logic::heal_range);

			// store new heal cost
			heal2.cost = [=](const SkillState&) { return cost; };

			// check if enough resources are present
			return state.with(heal2.resource) >= cost;
		},
		.radius = (size_t)logic::heal_range,
		.action = [](const SkillState&, Map& map, const HexRef& _, const HexRef& tile) -> Move* {
			// ignore if no building
			if (!tile.hex->build) return nullptr;

			// create heal move
			return new Moves::TroopHeal(
				tile.pos, logic::heal_range,
				logic::heal_amount[1], tile.hex->team
			);
		},
		.format = Skill::Self,
		.cooldown = 2
	};

	/// ======== BEACON STUN ======== ///
	const Skill stun = {
		.type = Skills::Stun,
		.annotation = Skill::Peach,
		.resource = Skills::Peach,
		.cost = skillf::cost(20),
		.radius = 3,
		.action = [](const SkillState&, Map& map, const HexRef& _, const HexRef& tile) -> Move* {
			// ignore if no building
			if (!tile.hex->build) return nullptr;

			// create movement move
			return new Moves::RadiusEffect(
				tile.pos, stun.radius,
				EffectType::Stunned, tile.hex->team
			);
		},
		.format = Skill::Self,
		.cooldown = 3
	};
};