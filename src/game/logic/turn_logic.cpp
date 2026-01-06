#include "game/logic/turn_logic.hpp"

namespace logic {
	/// Executes turn transition logic.
	/// 
	/// @param map Map reference.
	History turn(Map* map) {
		// temporary history object
		History history(map);
		
		// update region money
		auto regs = map->regions.iter();
		while (auto* reg = regs.next())
			reg->tick();

		// tick all troops
		auto troops = map->troopList();
		while (auto* troop = troops.next()) {
			troop->tickState();

			// @todo cache into a move
		};

		// return turn history
		return history;
	};
};