#pragma once

// map forward declaration
class Map;

/// Abstract reversible game move.
class Move {
public:
	/// Applies the move.
	///
	/// @param map Game map reference.
	virtual void apply(Map* map) = 0;

	/// Reverts the move.
	///
	/// @param map Game map reference.
	virtual void revert(Map* map) = 0;
};

/// Specific moves namespace.
namespace Moves {};