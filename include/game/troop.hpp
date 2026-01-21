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
	Damage offense(Access acc) override;
	/// Defends the troop against incoming damage.
	Damage defend(Damage dmg, Access acc) override;

	/// Returns troop skill at index.
	Skills::Type skill_at(int idx) const override;
	/// Returns troop targeted skill index.
	Skills::Type skill_into(const Entity* entity) const override;
};