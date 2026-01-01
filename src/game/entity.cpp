#include "game/entity.hpp"

/// Ticks entity state.
void Entity::tick() {
	// tick timers
	for (int i = 0; i < 4; i++) {
		if (timers[i] > 0)
			timers[i]--;
	};
};

/// Returns maximum entity hitpoints.
int Entity::max_hp() const {
	return 1;
};

/// Returns current entity damage.
int Entity::offense() {
	return 0;
};

/// Returns current entity power level.
int Entity::power() {
	return 0;
};

/// Deals damage to the entity.
int Entity::damage(int pts, int pow) {
	hp -= pts;
	return pts;
};

/// Checks whether the entity is dead.
bool Entity::dead() const {
	return hp <= 0;
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
bool Entity::removeEffect(EffectType effect) {
	for (auto it = _effects.begin(); it != _effects.end(); it++) {
		if (*it == effect) {
			_effects.erase(it);
			return true;
		};
	};
	return false;
};

/// Checks whether the entity has an effect applied.
bool Entity::hasEffect(EffectType effect) const {
	for (EffectType applied : _effects)
		if (applied == effect)
			return true;
	return false;
};

/// Overwrites entity effect list.
void Entity::setEffects(const Effects& list) {
	_effects = list;
};

/// Returns applied effect list.
const Entity::Effects& Entity::effectList() const {
	return _effects;
};