#pragma once

// include dependencies
#include "entity.hpp"

/// Plant object.
/// Stores a state of a plant.
struct Plant : Entity {
	/// Plant type.
	enum Type {
		Bush,    /// Bush.
		Berry,   /// Bush with berries.
		Sapling, /// Peach sapling.
		Tree,    /// Peach tree.
		Peach,   /// Peach tree with peaches.
		Pine,    /// Pine tree.

		Count    /// Plant count.
	} type;
};

namespace Name {
	/// Returns a request value for plant name.
	/// 
	/// @param id Plant ID.
	const char* plant(Plant::Type id);
	/// Returns a request value for plant name.
	/// 
	/// @param entity Plant entity.
	const char* plant(const Plant& entity);
};