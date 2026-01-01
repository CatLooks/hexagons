#include "game/entity.hpp"

/// Ticks entity state.
void Entity::tick() {
	// tick timers
	for (int i = 0; i < 4; i++) {
		if (timers[i] > 0)
			timers[i]--;
	};
};

/// Adds cooldown to entity skill if present.
void Entity::add_cooldown(Skills::Type skill, uint8_t time) {
	int id = skill_id(skill);
	if (id >= 0 && id < 4) timers[id] += time;
};
/// Subtracts cooldown from entity skill if present.
void Entity::sub_cooldown(Skills::Type skill, uint8_t time) {
	int id = skill_id(skill);
	if (id >= 0 && id < 4) timers[id] -= time;
};

/// Returns maximum entity hitpoints.
int Entity::max_hp() const { return 1; };

/// Returns current entity damage.
Entity::Damage Entity::offense(Access acc) { return {}; };
/// Defends against incoming damage.
Entity::Damage Entity::defend(Damage dmg, Access acc) { return dmg; };

/// Deals damage to the entity.
Entity::Damage Entity::damage(Damage dmg) {
	dmg = defend(dmg, Access::Use);
	hp -= dmg.pts;
	return dmg;
};

/// Checks whether the entity is dead.
bool Entity::dead() const { return hp <= 0; };

/// Checks whether the entity can be instakilled.
bool Entity::instakill(Damage dmg) {
	return hp - defend(dmg, Access::Query).pts <= 0;
};

/// Returns index of an entity skill.
int Entity::skill_id(Skills::Type skill) const { return -1; };

/// Returns index of an entity skill for targeting another entity.
Skills::Type Entity::skill_into(const Entity* entity) const { return Skills::Empty; };

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

/// Checks whether the entity has an effect applied.
bool Entity::useEffect(EffectType effect, Access acc) {
	return acc == Access::Use
		? removeEffect(effect)
		: hasEffect(effect);
};

/// Overwrites entity effect list.
void Entity::setEffects(const Effects& list) {
	_effects = list;
};

/// Returns applied effect list.
const Entity::Effects& Entity::effectList() const {
	return _effects;
};