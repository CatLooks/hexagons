#include "game/troop.hpp"
#include "game/build.hpp"
#include "game/plant.hpp"
#include "game/logic/troop_logic.hpp"

/// Returns troop maximum hitpoints.
int Troop::max_hp() const {
	return logic::troop_hp[type];
};

/// Returns troop damage.
Entity::Damage Troop::offense(Access acc) {
	// get base damage
	int pts = logic::troop_dmg[type];
	
	// apply boosts
	if (useEffect(EffectType::OffenseBoost, acc))
		pts *= 5;

	// return damage
	return { pts, logic::troop_pow[type] };
};

/// Defends the troop against incoming damage.
Entity::Damage Troop::defend(Damage dmg, Access acc) {
	// get own power level
	auto own = logic::troop_pow[type];

	// apply debuffs
	if (useEffect(EffectType::Shielded, acc))
		dmg.pts /= 2;
	if (useEffect(EffectType::DefenseBoost, acc))
		dmg.pts /= 10;
	
	// cap to 1 damage
	if (dmg.pow >= own && dmg.pts < 1)
		dmg.pts = 1;
	return dmg;
};

/// Returns troop skill index.
int Troop::skill_id(Skills::Type skill) const {
	// ignore empty skill
	if (skill == Skills::Empty) return -1;

	// check each skill slot
	for (int i = 0; i < 4; i++) {
		if (logic::troop_skills[type].skills[i]->type == skill)
			return i;
	};

	// not found
	return -1;
};

/// Returns troop targeted skill index.
Skills::Type Troop::skill_into(const Entity* entity) const {
	// troop target
	if (auto* oth = dynamic_cast<const Troop*>(entity)) {
		switch (type) {
			case Lumberjack: return Skills::AttackLumber;
			case Spearman  : return Skills::AttackSpear;
			case Archer    : return Skills::AttackArcher;
			case Baron     : return Skills::AttackBaron;
			case Knight    : return Skills::AttackKnight;
		};
	};

	// plant target
	if (auto* oth = dynamic_cast<const Plant*>(entity))
		if (type == Lumberjack) return Skills::TreeCut;

	// unknown target
	return Skills::Empty;
};