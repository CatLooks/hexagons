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

namespace Name {
	/// Returns a request value for troop name.
	/// 
	/// @param id Troop ID.
	const char* troop(Troop::Type id);
	/// Returns a request value for troop name.
	/// 
	/// @param entity Troop entity.
	const char* troop(const Troop& entity);
};