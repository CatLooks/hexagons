#pragma once

/// Effect enumeration.
enum class EffectType {
	Shielded,     /// Shield for a spearman.
	RangeBoost,   /// Range boost for an archer.
	DefenseBoost, /// Defense boost for a baron.
	OffenseBoost, /// Attack boost for a knight.
	Poisoned,     /// Poisoned by an archer.
	Stunned,      /// Stunned by a beacon.
	Enchant,      /// Next attack is poisoned.
	Count
};