#include "game/moves/game_turn.hpp"
#include "game/map.hpp"

namespace Moves {
	/// Constructs an entity change move.
	EntityChange::EntityChange(EntState prev):
		state(Empty{ .pos = entity_pos(&prev) }), a_prev(prev) {};

	/// Applies the move.
	void EntityChange::onApply(Map* map) {
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
	void EntityChange::onRevert(Map* map) {
		// get tile
		Hex* hex = map->at(entity_pos(&state));
		if (!hex) return;

		// restore entity
		place_entity(&a_prev, map);
	};

	/// Constructs a region change move.
	RegionChange::RegionChange(sf::Vector2i pos, Region prev) :
		state{}, a_prev(prev) { skill_pos = pos; };

	/// Applies the move.
	void RegionChange::onApply(Map* map) {
		// get tile
		Hex* hex = map->at(skill_pos);
		if (!hex) return;

		// override region state
		if (hex->region())
			*hex->region() = state;
	};

	/// Reverts the move.
	void RegionChange::onRevert(Map* map) {
		// get tile
		Hex* hex = map->at(skill_pos);
		if (!hex) return;

		// override region state
		if (hex->region())
			*hex->region() = a_prev;
	};
};