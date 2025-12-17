#include "game/skill.hpp"

/// Skill resource label text path.
const char* Skills::withLabel[3] = {
	"gp.cost.coins",
	"gp.cost.berry",
	"gp.cost.peach",
};

/// Returns the amount of a specified resource.
int SkillState::with(Skills::Resource resource) const {
	// fallback if no region selected
	if (!region) return 0;

	// select a resource
	switch (resource) {
		case Skills::Berry: return region->berry;
		case Skills::Peach: return region->peach;
		default: return 0;
	};
};