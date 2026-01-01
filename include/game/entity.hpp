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

/// Property access pattern.
///
/// Casting to `bool` will result in whether the access property should be updated.
enum class Access {
	Query = 0, /// Query access.
	Use   = 1, /// Consume access.
};

/// Entity object.
/// Stores common data shared between all entities.
struct Entity {
	/// Applied effect list type.
	using Effects = std::vector<EffectType>;

	/// Entity damage info.
	struct Damage {
		int pts = 0; /// Points of damage dealt.
		int pow = 0; /// Damage power level.
	};

public:
	/// Ticks entity state.
	void tick();

	/// Entity position on map.
	sf::Vector2i pos;

	/// Current hitpoints.
	int hp = max_hp();

	/// Entity skill timers.
	uint8_t timers[4] {0};

	/// Adds cooldown to entity skill if present.
	/// 
	/// @param skill Skill type.
	/// @param time Cooldown time.
	void add_cooldown(Skills::Type skill, uint8_t time);
	/// Subtracts cooldown from entity skill if present.
	/// 
	/// @param skill Skill type.
	/// @param time Cooldown time.
	void sub_cooldown(Skills::Type skill, uint8_t time);

	/// Returns maximum entity hitpoints.
	virtual int max_hp() const;

	/// Returns current entity damage.
	///
	/// @param acc Access pattern.
	virtual Damage offense(Access acc);

	/// Defends against incoming damage.
	/// 
	/// @param dmg Incoming damage.
	/// @param acc Access pattern.
	/// 
	/// @return Resulting damage taken.
	virtual Damage defend(Damage dmg, Access acc);

	/// Deals damage to the entity.
	/// 
	/// @param dmg Incoming damage.
	/// 
	/// @return Damage dealt.
	Damage damage(Damage dmg);

	/// Checks whether the entity can be instakilled.
	/// 
	/// @param dmg Incoming damage.
	bool instakill(Damage dmg);

	/// Checks whether the entity is dead.
	virtual bool dead() const;

	/// Returns index of an entity skill or `-1` if entity does not have the skill.
	/// 
	/// @param skill Skill type.
	virtual int skill_id(Skills::Type skill) const;
	/// Returns entity skill for targeting another entity.
	/// 
	/// @param entity Other entity pointer.
	virtual Skills::Type skill_into(const Entity* entity) const;

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

	/// Checks whether the entity has an effect applied.
	/// 
	/// With the `Use` access, removes the effect.
	/// 
	/// @param effect Effect type.
	/// @param acc Access pattern.
	/// 
	/// @return Whether the effect was being applied.
	bool useEffect(EffectType effect, Access acc);

	/// Overwrites entity effect list.
	/// 
	/// @param list New effect list.
	void setEffects(const Effects& list);

	/// Returns applied effect list.
	const Effects& effectList() const;
};