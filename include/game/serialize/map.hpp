#pragma once

// include dependencies
#include "general.hpp"
#include "game/hex.hpp"
#include "game/template.hpp"

namespace Serialize {
	/// Serializes a template signature.
	void encodeSignature(sf::Packet& packet);
	/// Deserializes a template signature.
	/// @return Whether a signature was recognized.
	bool decodeSignature(sf::Packet& packet);

	/// Serializes a hex base object.
	sf::Packet& operator<<(sf::Packet& packet, const HexBase& hex);
	/// Deserializes a hex base object.
	sf::Packet& operator>>(sf::Packet& packet, HexBase& hex);

	/// Serializes region resources object.
	sf::Packet& operator<<(sf::Packet& packet, const RegionRes& res);
	/// Deserializes region resources object.
	sf::Packet& operator>>(sf::Packet& packet, RegionRes& res);

	/// Serializes region variable counters object.
	sf::Packet& operator<<(sf::Packet& packet, const RegionVar& var);
	/// Deserializes region variable counters object.
	sf::Packet& operator>>(sf::Packet& packet, RegionVar& var);

	/// Serializes a map template object.
	sf::Packet& operator<<(sf::Packet& packet, const Template& temp);
	/// Deserializes a map template object.
	sf::Packet& operator>>(sf::Packet& packet, Template& temp);
};