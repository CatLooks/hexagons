#include "game/hex.hpp"

/// Adds the tile to the region.
void Hex::join(Regions::Ref ref) {
	// leave previous region
	leave();

	// join new region
	_region = ref;
	if (_region) {
		_region->tiles++;
	};
};

/// Removes the tile from the region.
void Hex::leave() {
	if (_region) {
		_region->tiles--;
	};

	// leave the region
	_region = {};
};

/// Returns current tile region.
const Regions::Ref& Hex::region() const {
	return _region;
};

/// Empty hex constructor.
Hex::Hex() = default;
/// Disabled move constructor.
Hex::Hex(Hex&&) noexcept = default;
/// Move assignment.
Hex& Hex::operator=(Hex&& hex) noexcept = default;

/// Checks if a tile is elevated.
bool Hex::elevated() const {
	return elevation != 0.0f;
};
/// Checks if a tile is ground-like.
bool Hex::solid() const {
	return type == Ground || type == Bridge;
};

/// Checks if a tile does not contain any entities.
bool HexEnt::free() const {
	return !troop && !build && !plant;
};

/// Returns a reference to tile entity.
Entity* HexEnt::entity() const {
	if (troop) return &*troop;
	if (build) return &*build;
	if (plant) return &*plant;
	return nullptr;
};

/// Constructs an empty reference.
HexRef::HexRef() : pos{}, hex{} {};

/// Constructs a reference directly from raw data.
/// 
/// @param hex Hex reference.
/// @param pos Hex position.
HexRef::HexRef(Hex* hex, sf::Vector2i pos) : pos(pos), hex(hex) {};