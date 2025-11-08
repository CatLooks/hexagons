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
		Grave,   /// Grave.

		Count    /// Plant count.
	} type;
};