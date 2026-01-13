#pragma once

// include dependencies
#include "general.hpp"
#include "game/hex.hpp"

namespace Serialize {
	/// Serializes a hex object.
	sf::Packet& operator<<(sf::Packet& packet, const Hex& hex);
	/// Deserializes a hex object.
	sf::Packet& operator>>(sf::Packet& packet, Hex& hex);

	/// Serializes region resources object.
	sf::Packet& operator<<(sf::Packet& packet, const RegionRes& res);
	/// Deserializes region resources object.
	sf::Packet& operator>>(sf::Packet& packet, RegionRes& res);

	/// Serializes region variable counters object.
	sf::Packet& operator<<(sf::Packet& packet, const RegionVar& var);
	/// Deserializes region variable counters object.
	sf::Packet& operator>>(sf::Packet& packet, RegionVar& var);

	/// Serializes a region object.
	sf::Packet& operator<<(sf::Packet& packet, const Region& reg);
	/// Deserializes a region object.
	sf::Packet& operator>>(sf::Packet& packet, Region& reg);
};