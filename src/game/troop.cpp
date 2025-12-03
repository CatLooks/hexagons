#include "game/troop.hpp"

/// Applies an effect to the troop.
void Troop::addEffect(EffectType effect) {
	if (!hasEffect(effect))
		effects.push_back(effect);
};

/// Checks whether a troop has an effect applied.
bool Troop::hasEffect(EffectType effect) const {
	for (auto item : effects)
		if (item == effect)
			return true;
	return false;
};