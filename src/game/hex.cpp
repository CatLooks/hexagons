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

/// Returns a reference to tile entity.
Entity* Hex::entity() const {
	if (troop) return &*troop;
	if (build) return &*build;
	if (plant) return &*plant;
	return nullptr;
};

/// Extends entity cooldown on this hex.
void Hex::add_cooldown(Skills::Type skill, uint8_t cd) const {
	// get entity
	auto* e = entity();
	if (!e) return;

	// get skill index
	int idx = e->skill_id(skill);
	if (idx == -1) return;

	// add cooldown
	e->timers[idx] += cd;
};

/// Shortens entity cooldown on this hex.
void Hex::sub_cooldown(Skills::Type skill, uint8_t cd) const {
	// get entity
	auto* e = entity();
	if (!e) return;

	// get skill index
	int idx = e->skill_id(skill);
	if (idx == -1) return;

	// add cooldown
	e->timers[idx] -= cd;
};

/// Constructs an empty reference.
HexRef::HexRef() : pos{}, hex{} {};

/// Constructs a reference directly from raw data.
/// 
/// @param hex Hex reference.
/// @param pos Hex position.
HexRef::HexRef(Hex* hex, sf::Vector2i pos) : pos(pos), hex(hex) {};