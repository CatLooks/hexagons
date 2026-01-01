#pragma once

// include dependencies
#include "entity.hpp"

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

	/// Returns troop maximum hitpoints.
	int max_hp() const override;

	/// Returns troop damage.
	int offense() override;
	/// Returns troop power level.
	int power() override;
	/// Deals damage to the troop.
	int damage(int pts, int pow) override;

	/// Returns troop skill index.
	int skill_id(Skills::Type skill) const override;
};