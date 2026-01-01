#include "game/moves/troop_attack.hpp"
#include "game/map.hpp"

namespace Moves {
	/// Constructs a troop attack move.
	TroopAttack::TroopAttack(sf::Vector2i dest)
		: dest(dest), a_dmg(0), a_state({}), a_eff({}) {};

	/// Applies the move.
	void TroopAttack::onApply(Map* map) {
		// get tiles
		auto from = map->atref(skill_pos);
		auto to = map->atref(dest);
		if (!(from.hex && to.hex)) return;

		// get entity pointers
		Entity* src = from.hex->entity();
		Entity* dst = to.hex->entity();
		if (!(src && dst)) return;

		// store attacked entity state
		if (to.hex->troop) a_state = *to.hex->troop;
		else if (to.hex->build) a_state = *to.hex->build;
		else if (to.hex->plant) a_state = *to.hex->plant;

		// store attacking entity effects
		a_eff = src->effectList();

		// remove range boost
		src->removeEffect(EffectType::RangeBoost);

		// deal damage to entity
		a_dmg = dst->damage(src->offense(Access::Use));

		// replace by a grave if dead
		if (dst->dead()) {
			Plant grave;
			grave.pos = to.pos;
			grave.type = Plant::Grave;
			map->setPlant(grave);
		};
	};

	/// Reverts the move.
	void TroopAttack::onRevert(Map* map) {
		// get tiles
		auto from = map->atref(skill_pos);
		auto to = map->atref(dest);
		if (!(from.hex && to.hex)) return;

		// restore effects
		if (from.hex->troop)
			from.hex->troop->setEffects(a_eff);

		// restore previous entity
		if (auto* data = std::get_if<Troop>(&a_state))
			map->setTroop(*data);
		if (auto* data = std::get_if<Build>(&a_state))
			map->setBuild(*data);
		if (auto* data = std::get_if<Plant>(&a_state))
			map->setPlant(*data);
	};

	/// Emits move section info.
	void TroopAttack::emitDev(dev::Section* section, ui::Text::List& list) const {
		// construct new fields
		section->line("dp.move.override");
		section->line("dp.move.troop_attack.dmg");
		section->extra("dp.move.troop_attack.to");
		section->line("dp.move.troop_attack.effects");

		// add arguments
		list["pos"] = ext::str_vec(dest);
		list["entity"] = str_ent(&a_state);
		list["dmg"] = std::format("{} @ {}!", a_dmg.pts, a_dmg.pow);
		list["effects"] = str_eff(a_eff);
		list["skill_name"] = "@!dp.move.name.troop_move";
	};
};