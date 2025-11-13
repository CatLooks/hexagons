#pragma once

// include dependencies
#include "troop.hpp"
#include "build.hpp"
#include "plant.hpp"

/// Hex tile.
/// Contains tile status and reference to objects placed on it.
struct Hex {
	/// Tile type.
	enum Type {
		Void,   /// Void tile.
		Water,  /// Water tile, can be turned into a bridge.
		Ground, /// Ground tile, can be captured.
		Bridge, /// Bridge tile, combo of `Water` and `Ground` properties.
	} type = Void;

	/// Current tile owner team.
	enum Team {
		/// (0) No owner.
		/// Can also be used as "void" team to simplify code.
		Unclaimed = 0,

		Red,    /// (1) Red team.
		Orange, /// (2) Orange team.
		Yellow, /// (3) Yellow team.
		Green,  /// (4) Green team.
		Aqua,   /// (5) Aqua team.
		Blue,   /// (6) Blue team.
		Purple, /// (7) Purple team.
		Pink,   /// (8) Pink team.

		Count   /// Team count.
	} team = Unclaimed;

	/// Troop reference, `nullptr` if no troop.
	Troop* troop = nullptr;
	/// Building reference, `nullptr` if no building.
	Build* build = nullptr;
	/// Plant reference, `nullptr` if no plant.
	Plant* plant = nullptr;

	/// Checks if a tile can be stood on.
	bool solid() const;
};