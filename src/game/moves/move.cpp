#include "game/moves/move.hpp"
#include "game/map.hpp"

/// Applies the move.
void Move::apply(Map* map) {
	// add cooldown
	if (skill_cooldown) {
		Hex* hex = map->at(skill_pos);
		if (hex) hex->add_cooldown(skill_type, skill_cooldown);
	};

	// apply the move
	onApply(map);
};

/// Reverts the move.
void Move::revert(Map* map) {
	// apply the move
	onRevert(map);

	// remove cooldown
	if (skill_cooldown) {
		Hex* hex = map->at(skill_pos);
		if (hex) hex->sub_cooldown(skill_type, skill_cooldown);
	};
};