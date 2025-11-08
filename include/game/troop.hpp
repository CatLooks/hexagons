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

	int hp = 0; /// Troop health.
};