#pragma once

// include dependencies
#include "troop.hpp"
#include "build.hpp"
#include "plant.hpp"
#include "region.hpp"
#include <pool>
#include <refpool>

/// Base hex state.
struct HexBase {
	/// Tile type.
	enum Type {
		Void,   /// Void tile.
		Water,  /// Water tile, can be turned into a bridge.
		Ground, /// Ground tile, can be captured.
		Bridge, /// Bridge tile, combo of `Water` and `Ground` properties.
	} type = Void;

	/// Tile team.
	Region::Team team = Region::Unclaimed;
};

/// References to entities on the hex.
struct HexEnt {
	Pool<Troop>::Item troop; /// Troop reference, `null` if no troop.
	Pool<Build>::Item build; /// Building reference, `null` if no building.
	Pool<Plant>::Item plant; /// Plant reference, `null` if no plant.

	/// Checks if a tile does not contain any entities.
	bool free() const;

	/// Returns a reference to tile entity or `null` if no entity is present.
	Entity* entity() const;
};

/// Hex tile.
/// Contains tile status and reference to objects placed on it.
struct Hex : HexBase, HexEnt {
	using HexBase::Type;

	float elevation = 0; /// Tile elevation.
	size_t selected = 0; /// Selection index.
	size_t spread   = 0; /// Spread buffer index.

private:
	Regions::Ref _region; /// Tile region reference.

public:
	/// Adds the tile to the region.
	///
	/// @param ref Region reference.
	void join(Regions::Ref ref);

	/// Removes the tile from the region.
	void leave();

	/// Returns current tile region.
	const Regions::Ref& region() const;

public:
	/// Empty hex constructor.
	Hex();
	/// Disabled move constructor.
	Hex(Hex&&) noexcept;
	/// Move assignment.
	Hex& operator=(Hex&&) noexcept;

	/// Checks if a tile is elevated.
	bool elevated() const;
	/// Checks if a tile can be stood on.
	bool solid() const;
};

/// Hex reference.
struct HexRef {
	sf::Vector2i pos {}; /// Hex position.
	Hex*         hex {}; /// Hex pointer.

	/// Constructs an empty reference.
	HexRef();

	/// Constructs a reference directly from raw data.
	/// 
	/// @param hex Hex reference.
	/// @param pos Hex position.
	HexRef(Hex* hex, sf::Vector2i pos);
};