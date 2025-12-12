#include "game/troop.hpp"
#include "game/logic/troop_logic.hpp"

/// Returns troop maximum hitpoints.
int Troop::max_hp() const {
	return logic::troop_hp[type];
};