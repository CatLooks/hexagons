#include "game/moves/game_turn.hpp"
#include "game/map.hpp"

namespace Moves {
	/// Constructs a game turn move.
	///
	/// @param prev Previous entity state.
	GameTurn::GameTurn(EntState prev):
		state(Empty{ .pos = entity_pos(&prev) }), a_prev(prev) {};

	/// Applies the move.
	void GameTurn::onApply(Map* map) {
		// get tile
		sf::Vector2i pos = entity_pos(&state);
		Hex* hex = map->at(pos);
		if (!hex) return;

		// store previous state
		a_prev = store_entity(hex, pos);

		// override entity
		place_entity(&state, map);
	};

	/// Reverts the move.
	void GameTurn::onRevert(Map* map) {
		// get tile
		Hex* hex = map->at(entity_pos(&state));
		if (!hex) return;

		// restore entity
		place_entity(&a_prev, map);
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
		list["skill_pos"] = ext::str_vec(entity_pos(&state));
	};
};