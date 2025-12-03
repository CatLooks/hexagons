#include "game/hex.hpp"

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
bool Hex::free() const {
	return !troop && !build && !plant;
};

/// Constructs an empty reference.
HexRef::HexRef() : pos{}, hex{} {};

/// Constructs a reference directly from raw data.
/// 
/// @param hex Hex reference.
/// @param pos Hex position.
HexRef::HexRef(Hex* hex, sf::Vector2i pos) : pos(pos), hex(hex) {};