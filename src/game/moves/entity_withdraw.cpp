#include "game/moves/entity_withdraw.hpp"
#include "game/map.hpp"

namespace Moves {
	/// Constructs entity withdraw move.
	EntityWithdraw::EntityWithdraw(sf::Vector2i pos)
		: pos(pos), a_state{} {};

	/// Applies the move.
	void EntityWithdraw::onApply(Map* map) {
		// get entity at position
		Hex* hex = map->at(pos);
		if (!hex) return;
		
		// store entity state
		if (hex->troop) a_state = *hex->troop;
		else if (hex->build) a_state = *hex->build;
		else if (hex->plant) a_state = *hex->plant;

		// withdraw entity
		map->removeEntity(hex);
	};

	/// Reverts the move.
	void EntityWithdraw::onRevert(Map* map) {
		// get entity at position
		Hex* hex = map->at(pos);
		if (!hex) return;

		// restore previous entity
		if (auto* troop = std::get_if<Troop>(&a_state))
			map->setTroop(*troop);
		if (auto* build = std::get_if<Build>(&a_state))
			map->setBuild(*build);
		if (auto* plant = std::get_if<Plant>(&a_state))
			map->setPlant(*plant);
	};

	/// Emits move section info.
	void EntityWithdraw::emitDev(dev::Section* section, ui::Text::List& list) const {
		// construct new fields
		section->line("dp.move.override");

		// add arguments
		list["entity"] = str_ent(&a_state);
		list["skill_name"] = "@!dp.move.name.entity_withdraw";
	};
};