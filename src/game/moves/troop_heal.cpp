#include "game/moves/troop_heal.hpp"
#include "game/logic/skill_helper.hpp"
#include "game/map.hpp"

#include "game/values/hex_values.hpp"

namespace Moves {
	/// Generates a troop heal cost checker.
	Spread troopHealCost(int heal, Region::Team team, int* cost) {
		return {
			.hop = skillf::solidHop,
			// troops with some of hp missing
			.pass = [=](const Spread::Tile& tile) {
				// ignore if no same-team troop
				if (!(tile.hex->team == team && tile.hex->troop))
					return false;

				// get missing hp
				int diff = tile.hex->troop->max_hp() - tile.hex->troop->hp;
				if (diff <= 0)
					return false;

				// add heal cost
				*cost += ext::imin(diff, heal) * logic::berry_per_hp;
				return true;
			}
		};
	};

	/// Constructs a troop heal move.
	TroopHeal::TroopHeal(sf::Vector2i mid, size_t radius, int heal, Region::Team team)
		: radius(radius), mid(mid), heal(heal), team(team), a_cost{}
	{
		spread = troopHealCost(heal, team, &a_cost);
		spread.effect = [=](const Spread::Tile& tile) {
			// add heal targets to list
			a_target.push_back({ tile.pos, tile.hex->troop->hp });
		};
	};

	/// Applies the move.
	void TroopHeal::onApply(Map* map) {
		// fetch target list
		a_cost = 0;
		a_target.clear();
		spread.apply(*map, mid, radius);

		// subtract healing cost
		Hex* hex = map->at(mid);
		if (hex && hex->region())
			hex->region()->berry -= a_cost;

		// heal all targets
		for (const auto& target : a_target) {
			// get hex
			Hex* hex = map->at(target.pos);
			if (!hex) continue;

			// apply heal
			if (hex->troop) hex->troop->hp = ext::imin(
				hex->troop->hp + heal,
				hex->troop->max_hp()
			);
		};
	};

	/// Reverts the move.
	void TroopHeal::onRevert(Map* map) {
		// revert hp for all targets
		for (const auto& target : a_target) {
			// get hex
			Hex* hex = map->at(target.pos);
			if (!hex) continue;

			// revert hp
			if (hex->troop) hex->troop->hp = target.hp_before;
		};

		// recompensate healing cost
		Hex* hex = map->at(mid);
		if (hex && hex->region())
			hex->region()->berry += a_cost;
	};

	/// Emits move section info.
	void TroopHeal::emitDev(dev::Section* section, ui::Text::List& list) const {
		// construct new fields
		section->line("dp.move.troop_heal.heal");
		section->line("dp.move.troop_heal.spread");

		// add arguments
		list["hp"] = ext::str_int(heal);
		list["pos"] = ext::str_vec(mid);
		list["radius"] = ext::str_int(radius);
		list["team"] = Values::hex_names[team];
		list["cost"] = ext::str_int(a_cost);
		list["count"] = ext::str_int(a_target.size());
		list["skill_name"] = "@!dp.move.name.troop_heal";
	};
};