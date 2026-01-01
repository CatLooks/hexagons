#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include <stdint.h>
#include <vector>
#include "logic/skill_types.hpp"

/// Effect enumeration.
enum class EffectType {
	Shielded,     /// Shield for a spearman.
	RangeBoost,   /// Range boost for an archer.
	DefenseBoost, /// Defense boost for a baron.
	OffenseBoost, /// Attack boost for a knight.
	Poisoned,     /// Poisoned by an archer.
	Stunned,      /// Stunned by a beacon.
	Count
};

/// Entity object.
/// Stores common data shared between all entities.
struct Entity {
	/// Applied effect list type.
	using Effects = std::vector<EffectType>;

public:
	/// Ticks entity state.
	void tick();

	/// Entity position on map.
	sf::Vector2i pos;

	/// Current hitpoints.
	int hp = max_hp();

	/// Entity skill timers.
	uint8_t timers[4] {0};

	/// Returns maximum entity hitpoints.
	virtual int max_hp() const;

	/// Returns current entity damage.
	virtual int offense();
	/// Returns current entity power level.
	virtual int power();
	/// Deals damage to the entity.
	/// 
	/// @param pts Attack damage.
	/// @param pow Attack power level.
	/// 
	/// @return Actual amount of damage dealt.
	virtual int damage(int pts, int pow);

	/// Checks whether the entity is dead.
	virtual bool dead() const;

	/// Returns index of an entity skill or `-1` if entity does not have the skill.
	/// 
	/// @param skill Skill type.
	virtual int skill_id(Skills::Type skill) const;

private:
	/// Applied effect list.
	Effects _effects;

public:
	/// Applies an effect to the entity.
	///
	/// @param effect Effect type.
	void addEffect(EffectType effect);

	/// Removes an effect from the entity.
	/// 
	/// @param effect Effect type.
	/// 
	/// @return Whether the effect was removed.
	bool removeEffect(EffectType effect);

	/// Checks whether the entity has an effect applied.
	///
	/// @param effect Effect type.
	bool hasEffect(EffectType effect) const;

	/// Overwrites entity effect list.
	/// 
	/// @param list New effect list.
	void setEffects(const Effects& list);

	/// Returns applied effect list.
	const Effects& effectList() const;
};