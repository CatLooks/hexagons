#include "game/troop.hpp"
#include "game/logic/troop_logic.hpp"

/// Returns troop maximum hitpoints.
int Troop::max_hp() const {
	return logic::troop_hp[type];
};

/// Returns troop skill index.
int Troop::skill_id(Skills::Type skill) const {
	for (size_t i = 0; i < 4; i++) {
		if (logic::troop_skills[type].skills[i]->type == skill)
			return (int)i;
	};
	return -1;
};