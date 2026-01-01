#pragma once

// include dependencies
#include "entity.hpp"

/// Building object.
/// Stores a state of a building.
struct Build : Entity {
	/// Building type.
	enum Type {
		Castle, /// Town hall (resource storage).
		Farm,   /// Farm (passive income).
		Mine,   /// Mine (uncaptured hex income).
		Tower,  /// Watchtower (basic defense).
		Beacon, /// Beacon (stunning defense).
		Fort,   /// Fortress (strong defense).

		Count   /// Building count.
	} type = Castle;

	/// Deals damage to the building.
	int damage(int pts, int pow) override;

	/// Returns building skill index.
	int skill_id(Skills::Type skill) const override;
};