#include "game/moves/radius_effect.hpp"
#include "game/map.hpp"

#include "game/values/entity_values.hpp"
#include "game/values/hex_values.hpp"

namespace Moves {
	/// Constructs an AoE effect move.
	RadiusEffect::RadiusEffect(sf::Vector2i mid, size_t radius, EffectType effect, Region::Team team)
		: effect(effect), mid(mid), radius(radius), team(team)
	{
		spread = {
			// unaffected troops of enemy teams
			.pass = [=](const Spread::Tile& tile) {
				return tile.hex->troop
					&& tile.hex->team != team
					&& !tile.hex->troop->hasEffect(effect);
			},
			// add troop to target list
			.effect = [=](const Spread::Tile& tile) {
				a_target.push_back(tile.pos);
			}
		};
	};

	/// Applies the move.
	void RadiusEffect::onApply(Map* map) {
		// fetch target list
		a_target.clear();
		spread.apply(*map, mid, radius);

		// apply effects to all target troops
		for (sf::Vector2i target : a_target) {
			// get hex
			Hex* hex = map->at(target);
			if (!hex) continue;

			// apply effect
			if (hex->troop) hex->troop->addEffect(effect);
		};
	};

	/// Reverts the move.
	void RadiusEffect::onRevert(Map* map) {
		// revert effect from all target troops
		for (sf::Vector2i target : a_target) {
			// get hex
			Hex* hex = map->at(target);
			if (!hex) continue;

			// revert effect
			if (hex->troop) hex->troop->removeEffect(effect);
		};
	};

	/// Emits move section info.
	void RadiusEffect::emitDev(dev::Section* section, ui::Text::List& list) const {
		// construct new fields
		section->line("dp.move.aoe_effect.effect");
		section->line("dp.move.aoe_effect.spread");

		// add arguments
		list["effect"] = Values::effect_names[static_cast<int>(effect)];
		list["pos"] = ext::str_vec(mid);
		list["radius"] = ext::str_int(radius);
		list["team"] = Values::hex_names[team];
		list["count"] = ext::str_int(a_target.size());
		list["skill_name"] = "@!dp.move.name.aoe_effect";
	};
};