#pragma once

// include dependencies
#include "troop.hpp"
#include "build.hpp"
#include "plant.hpp"
#include "region.hpp"
#include <pool>
#include <refpool>

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

	/// Tile team.
	Region::Team team = Region::Unclaimed;

	float elevation = 0; /// Tile elevation.
	size_t selected = 0; /// Selection index.
	size_t spread   = 0; /// Spread buffer index.
	Regions::Ref region; /// Tile region reference.

	Pool<Troop>::Item troop; /// Troop reference, `null` if no troop.
	Pool<Build>::Item build; /// Building reference, `null` if no building.
	Pool<Plant>::Item plant; /// Plant reference, `null` if no plant.

	/// Empty hex constructor.
	Hex();
	/// Disabled move constructor.
	Hex(Hex&&) noexcept;
	/// Move assignment.
	Hex& operator=(Hex&& hex) noexcept;

	/// Checks if a tile is elevated.
	bool elevated() const;
	/// Checks if a tile can be stood on.
	bool solid() const;
	/// Checks if a tile does not contain any entities.
	bool free() const;
};

/// Hex reference.
struct HexRef {
	sf::Vector2i pos {}; /// Hex position.
	Hex*         hex {}; /// Hex pointer.
};