#include "game/build.hpp"
#include "game/logic/build_logic.hpp"

/// Returns building skill index.
int Build::skill_id(Skills::Type skill) const {
	for (size_t i = 0; i < 4; i++) {
		if (logic::build_skills[type].skills[i]->type == skill)
			return (int)i;
	};
	return -1;
};