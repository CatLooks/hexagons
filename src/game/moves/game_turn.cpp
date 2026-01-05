#include "game/moves/game_turn.hpp"
#include "game/map.hpp"

namespace Moves {
	/// Applies the move.
	void GameTurn::onApply(Map* map) {
		// get position
		auto pos = entity_pos(&state);
		if (!pos) return;

		// get tile
		Hex* hex = map->at(*pos);
		if (!hex) return;

		// store previous state
		if (hex->troop) a_prev = *hex->troop;
		else if (hex->build) a_prev = *hex->build;
		else if (hex->plant) a_prev = *hex->plant;
		else a_prev = std::monostate();

		// override entity
		if (auto* data = std::get_if<Troop>(&state)) map->setTroop(*data);
		else if (auto* data = std::get_if<Build>(&state)) map->setBuild(*data);
		else if (auto* data = std::get_if<Plant>(&state)) map->setPlant(*data);
		else map->removeEntity(hex);
	};

	/// Reverts the move.
	void GameTurn::onRevert(Map* map) {
		// get position
		auto pos = entity_pos(&state);
		if (!pos) return;

		// get tile
		Hex* hex = map->at(*pos);
		if (!hex) return;

		// restore entity
		if (auto* data = std::get_if<Troop>(&a_prev)) map->setTroop(*data);
		else if (auto* data = std::get_if<Build>(&a_prev)) map->setBuild(*data);
		else if (auto* data = std::get_if<Plant>(&a_prev)) map->setPlant(*data);
		else map->removeEntity(hex);
	};

	/// Emits move section info.
	void GameTurn::emitDev(dev::Section* section, ui::Text::List& list) const {
		// construct new fields
		section->line("dp.move.game_turn.forward");
		section->line("dp.move.game_turn.backward");

		// add arguments
		list["forward"] = str_ent(&state);
		list["backward"] = str_ent(&a_prev);
		list["skill_name"] = "@!dp.move.name.game_turn";
	};
};