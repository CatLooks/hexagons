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
		Tent,   /// Tent (troop healing station).

		Count   /// Building count.
	} type = Castle;

	/// Return building's max hitpoints.
	int max_hp() const override;

	/// Defends the building against incoming damage.
	Damage defend(Damage dmg, Access acc) override;

	/// Returns building skill index.
	int skill_id(Skills::Type skill) const override;
};