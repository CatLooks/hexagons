#include "game/build.hpp"
#include "game/logic/build_logic.hpp"

/// Return building's max hitpoints.
int Build::max_hp() const {
	return logic::build_hp[type];
};

/// Defends the building against incoming damage.
Entity::Damage Build::defend(Damage dmg, Access acc) {
	// proc poison
	if (dmg.psn) {
		dmg.psn = false;
		dmg.pts++;
	};

	// round up
	if (dmg.pts & 1) dmg.pts++;
	// half the damage
	dmg.pts /= 2;

	// block heavy attack
	if (dmg.pts >= max_hp() * 2)
		dmg.pts /= 2;

	// cap at 1 damage
	if (dmg.pts <= 0)
		dmg.pts = 1;
	return dmg;
};

/// Returns building skill index.
Skills::Type Build::skill_at(int idx) const {
	return logic::build_skills[type].skills[idx]->type;
};