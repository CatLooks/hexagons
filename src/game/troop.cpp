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

	// poison the attack
	bool poison = useEffect(EffectType::Enchant, acc);
	if (acc == Access::Use && type == Archer && !poison)
		addEffect(EffectType::Enchant);

	// return damage
	return { pts, logic::troop_pow[type], poison };
};

/// Defends the troop against incoming damage.
Entity::Damage Troop::defend(Damage dmg, Access acc) {
	// get own power level
	auto own = logic::troop_pow[type];

	// apply debuffs
	if (useEffect(EffectType::Shielded, acc))
		dmg.pts /= 2;
	if (useEffect(EffectType::DefenseBoost, acc)) {
		dmg.pts /= 10;
		dmg.psn = false;
	};
	
	// cap to 1 damage
	if (dmg.pow >= own && dmg.pts < 1)
		dmg.pts = 1;
	return dmg;
};

/// Returns troop skill index.
Skills::Type Troop::skill_at(int idx) const {
	return logic::troop_skills[type].skills[idx]->type;
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