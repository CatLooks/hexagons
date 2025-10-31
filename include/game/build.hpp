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
	} type;
};

namespace Name {
	/// Returns a request value for building name.
	/// 
	/// @param id Building ID.
	const char* build(Build::Type id);
	/// Returns a request value for building name.
	/// 
	/// @param entity Building entity.
	const char* build(const Build& entity);
};