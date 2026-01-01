#include "game/troop.hpp"
#include "game/logic/troop_logic.hpp"

/// Returns troop maximum hitpoints.
int Troop::max_hp() const {
	return logic::troop_hp[type];
};

/// Returns troop damage.
int Troop::offense() {
	int pts = logic::troop_dmg[type];
	
	// apply boosts
	if (removeEffect(EffectType::OffenseBoost))
		pts *= 5;

	return pts;
};

/// Returns troop power level.
int Troop::power() {
	return logic::troop_pow[type];
};

/// Deals damage to the troop.
int Troop::damage(int pts, int pow) {
	auto own_pow = power();

	// apply debuffs
	if (removeEffect(EffectType::Shielded))
		pts /= 2;
	if (removeEffect(EffectType::DefenseBoost))
		pts /= 10;
	if (pow < own_pow)
		pts /= 2;

	// cap to 1 damage
	if (pow >= own_pow && pts < 1)
		pts = 1;

	// deal damage
	hp -= pts;
	return pts;
};

/// Returns troop skill index.
int Troop::skill_id(Skills::Type skill) const {
	for (size_t i = 0; i < 4; i++) {
		if (logic::troop_skills[type].skills[i]->type == skill)
			return (int)i;
	};
	return -1;
};