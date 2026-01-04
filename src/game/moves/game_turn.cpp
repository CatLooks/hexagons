#include "game/moves/game_turn.hpp"
#include "game/map.hpp"

namespace Moves {
	/// Applies changes from `src` and stores previous state to `dst`.
	/// 
	/// @param map Map reference.
	/// @param src List of forward changes.
	/// @param dst List of backward changes.
	static void gauss_shift(Map* map, const std::vector<EntState>* src, std::vector<EntState>* dst) {
		// override all entities
		for (const auto& state : *src) {
			// get position
			auto pos = entity_pos(&state);
			if (!pos) continue;

			// get tile
			Hex* hex = map->at(*pos);
			if (!hex) continue;

			// store previous state
			if (dst != nullptr) {
				if (hex->troop) dst->push_back(*hex->troop);
				else if (hex->build) dst->push_back(*hex->build);
				else if (hex->plant) dst->push_back(*hex->plant);
				else dst->push_back(std::monostate());
			};

			// override entity
			if (auto* data = std::get_if<Troop>(&state)) map->setTroop(*data);
			else if (auto* data = std::get_if<Build>(&state)) map->setBuild(*data);
			else if (auto* data = std::get_if<Plant>(&state)) map->setPlant(*data);
			else map->removeEntity(hex);
		};
	};

	/// Applies the move.
	void GameTurn::onApply(Map* map) {
		a_prev.clear();
		gauss_shift(map, &states, &a_prev);
	};

	/// Reverts the move.
	void GameTurn::onRevert(Map* map) {
		gauss_shift(map, &a_prev, nullptr);
	};

	/// Emits move section info.
	void GameTurn::emitDev(dev::Section* section, ui::Text::List& list) const {
		// construct new fields
		section->line("dp.move.game_turn.forward");
		section->line("dp.move.game_turn.backward");

		// add arguments
		list["forward"] = ext::str_int(states.size());
		list["backward"] = ext::str_int(a_prev.size());
		list["skill_name"] = "@!dp.move.name.game_turn";
	};
};