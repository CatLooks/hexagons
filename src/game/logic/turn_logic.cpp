#include "game/logic/turn_logic.hpp"

namespace logic {
	/// Executes turn transition logic.
	/// 
	/// @param map Map reference.
	History turn(Map* map) {
		// temporary history object
		History history(map);
		
		// tick all troops
		auto troops = map->troopList();
		while (auto troop = troops.next()) {
			troop->tick();
		};

		// return turn history
		return history;
	};
};