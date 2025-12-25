#include "game/entity.hpp"

/// Returns maximum entity hitpoints.
int Entity::max_hp() const {
	return 1;
};

/// Returns index of an entity skill.
int Entity::skill_id(Skills::Type skill) const {
	return -1;
};

/// Applies an effect to the entity.
void Entity::addEffect(EffectType effect) {
	if (!hasEffect(effect))
		_effects.push_back(effect);
};

/// Removes an effect from the entity.
void Entity::removeEffect(EffectType effect) {
	for (auto it = _effects.begin(); it != _effects.end(); it++) {
		if (*it == effect) {
			_effects.erase(it);
			return;
		};
	};
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

/// Ticks all skill timers.
void Entity::tickTimers() {
	for (int i = 0; i < 4; i++) {
		if (timers[i] > 0)
			timers[i]--;
	};
};