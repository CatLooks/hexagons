#pragma once

// include dependencies
#include "entity.hpp"
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

/// Troop object.
/// Stores a state of a troop.
struct Troop : Entity {
	/// Troop type.
	enum Type {
		Farmer,     /// Farmer troop (fruit harvesting troop).
		Lumberjack, /// Lumberjack (wood cutting troop).
		Spearman,   /// Spearman (stalling troop).
		Archer,     /// Archer (ranged troop).
		Baron,      /// Baron (defensive troop).
		Knight,     /// Knight (offensive troop).

		Count       /// Troop count.
	} type = Farmer;

	int hp     = 0; /// Troop health.

	/// Troop effects.
	std::vector<EffectType> effects;

	/// Applies an effect to the troop.
	/// 
	/// @param effect Effect type.
	void addEffect(EffectType effect);

	/// Checks whether a troop has an effect applied.
	/// 
	/// @param effect Effect type.
	bool hasEffect(EffectType effect) const;
};