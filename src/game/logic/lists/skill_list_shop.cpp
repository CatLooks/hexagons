#include "game/logic/skill_list.hpp"
#include "game/values/troop_values.hpp"
#include "game/values/build_values.hpp"

namespace SkillList {
	/// Selects tiles for region shop.
	static const Skill::Selection region_shop =
		[](const SkillState&, const HexRef& tile, size_t idx)
	{
		return Spread {
			.hop = skillf::sameRegionHop(tile.hex->region()),
			.pass = skillf::emptyPass,
			.effect = skillf::selectTile(idx),
			.imm = true
		};
	};

	/// Troop buy cost.
	static const Skill::Cost cost_troop =
		[](const SkillState& state)
	{
		return logic::troop_cost[Values::troop_shop[state.troop]];
	};

	/// Building buy cost.
	static const Skill::Cost cost_build =
		[](const SkillState& state)
	{
		return logic::build_cost(Values::build_shop[state.build], state);
	};

	/// Places a new troop.
	static const Skill::Action place_troop =
		[](const SkillState& state, Map& map, const HexRef& _, const HexRef& tile) -> Move*
	{
		// ignore if entity is present
		if (tile.hex->entity()) return nullptr;

		// create new troop
		Troop troop;
		troop.type = Values::troop_shop[state.troop];
		troop.hp = troop.max_hp();
		troop.add_cooldown(Skills::Withdraw, 1);

		// create placement move
		return new Moves::EntityPlace(troop, tile.pos, state);
	};

	/// Places a new building.
	static const Skill::Action place_build =
		[](const SkillState& state, Map& map, const HexRef& _, const HexRef& tile) -> Move*
	{
		// ignore if entity is present
		if (tile.hex->entity()) return nullptr;

		// create new troop
		Build build;
		build.type = Values::build_shop[state.build];
		build.hp = build.max_hp();
		build.add_cooldown(Skills::Withdraw, 1);

		// create placement move
		return new Moves::EntityPlace(build, tile.pos, state);
	};

	/// ======== BUY TROOP IN-PLACE ======== ///
	const Skill buy_troop = {
		.type = Skills::BuyTroop,
		.annotation = Skill::None,
		.resource = Skills::Money,
		.cost = cost_troop,
		.action = place_troop,
		.format = Skill::Self,
		.cooldown = 0
	};

	/// ======== BUY TROOP ======== ///
	const Skill buy_troop_aim = {
		.type = Skills::BuyTroop,
		.annotation = Skill::Aim,
		.resource = Skills::Money,
		.cost = cost_troop,
		.select = region_shop,
		.radius = ~0ull,
		.action = place_troop,
		.format = Skill::SingleAim,
		.cooldown = 0
	};

	/// ======== BUY BUILDING IN-PLACE ======== ///
	const Skill buy_build = {
		.type = Skills::BuyBuild,
		.annotation = Skill::None,
		.resource = Skills::Money,
		.cost = cost_build,
		.action = place_build,
		.format = Skill::Self,
		.cooldown = 0
	};

	/// ======== BUY BUILDING ======== ///
	const Skill buy_build_aim = {
		.type = Skills::BuyBuild,
		.annotation = Skill::Aim,
		.resource = Skills::Money,
		.cost = cost_build,
		.select = region_shop,
		.radius = ~0ull,
		.action = place_build,
		.format = Skill::SingleAim,
		.cooldown = 0
	};
};