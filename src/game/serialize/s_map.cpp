#include "game/serialize/map.hpp"

namespace Serialize {
	/// Serializes a hex object.
	sf::Packet& operator<<(sf::Packet& packet, const Hex& hex) {
		uint8_t byte = hex.team << 4 | hex.type;
		packet << byte;
		return packet;
	};
	/// Deserializes a hex object.
	sf::Packet& operator>>(sf::Packet& packet, Hex& hex) {
		auto byte = from<uint8_t>(packet);

		// read data
		hex.type = static_cast<Hex::Type>(byte & 0x3);
		hex.team = static_cast<Region::Team>(byte >> 4);

		// validate data
		if (hex.team >= Region::Count)
			hex.team = Region::Unclaimed;
		return packet;
	};

	/// Serializes region resources object.
	sf::Packet& operator<<(sf::Packet& packet, const RegionRes& res) {
		packet << res.money;
		packet << res.berry;
		packet << res.peach;
		return packet;
	};
	/// Deserializes region resources object.
	sf::Packet& operator>>(sf::Packet& packet, RegionRes& res) {
		packet >> res.money;
		packet >> res.berry;
		packet >> res.peach;
		return packet;
	};

	/// Serializes region variable counters object.
	sf::Packet& operator<<(sf::Packet& packet, const RegionVar& var) {
		packet << var.farms;
		packet << var.tents;
		return packet;
	};
	/// Deserializes region variable counters object.
	sf::Packet& operator>>(sf::Packet& packet, RegionVar& var) {
		packet >> var.farms;
		packet >> var.tents;
		return packet;
	};

	/// Serializes a region object.
	sf::Packet& operator<<(sf::Packet& packet, const Region& reg) {
		packet << (const RegionRes&)reg;
		packet << reg.income;
		packet << reg.tiles;
		packet << reg.farms;
		packet << reg.tents;
		return packet;
	};
	/// Deserializes a region object.
	sf::Packet& operator>>(sf::Packet& packet, Region& reg) {
		packet >> (RegionRes&)reg;
		packet >> reg.income;
		packet >> reg.tiles;
		packet >> reg.farms;
		packet >> reg.tents;
		return packet;
	};
};