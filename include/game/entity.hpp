#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include <stdint.h>
#include <vector>

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
	/// Entity position on map.
	sf::Vector2i pos;

	/// Current hitpoints.
	int hp = max_hp();

	/// Returns maximum entity hitpoints.
	virtual int max_hp() const;

private:
	/// Applied effect list.
	Effects _effects;

public:
	/// Applies an effect to the entity.
	///
	/// @param effect Effect type.
	void addEffect(EffectType effect);

	/// Checks whether the entity has an effect applied.
	///
	/// @param effect Effect type.
	bool hasEffect(EffectType effect) const;

	/// Returns applied effect list.
	const Effects& effectList() const;
};