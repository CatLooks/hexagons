#include "game/entity.hpp"

/// Returns maximum entity hitpoints.
int Entity::max_hp() const {
	return 1;
};

/// Applies an effect to the entity.
void Entity::addEffect(EffectType effect) {
	if (!hasEffect(effect))
		_effects.push_back(effect);
};

/// Checks whether the entity has an effect applied.
bool Entity::hasEffect(EffectType effect) const {
	for (EffectType applied : _effects)
		if (applied == effect)
			return true;
	return false;
};

/// Returns applied effect list.
const Entity::Effects& Entity::effectList() const {
	return _effects;
};