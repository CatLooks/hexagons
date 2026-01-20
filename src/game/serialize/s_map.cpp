#include "game/serialize/entities.hpp"
#include "game/serialize/map.hpp"

namespace Serialize {
	/// Map signature.
	static const uint8_t SIGN[4] = { 'h', 'e', 'x', '?' };

	/// Serializes a template signature.
	void encodeSignature(sf::Packet& packet) {
		for (int i = 0; i < 4; i++)
			packet << SIGN[i];
	};
	/// Deserializes a template signature.
	bool decodeSignature(sf::Packet& packet) {
		for (int i = 0; i < 4; i++) {
			if (from<uint8_t>(packet) != SIGN[i])
				return false;
		};
		return true;
	};

	/// Serializes a hex base object.
	sf::Packet& operator<<(sf::Packet& packet, const HexBase& hex) {
		uint8_t byte = hex.team << 4 | hex.type;
		packet << byte;
		return packet;
	};
	/// Deserializes a hex base object.
	sf::Packet& operator>>(sf::Packet& packet, HexBase& hex) {
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

	/// Serializes a map template object.
	sf::Packet& operator<<(sf::Packet& packet, const Template& temp) {
		// signature

		// map header
		packet << temp.header.name;
		packet << temp.header.auth;

		// tile data
		packet << temp.size();
		for (int y = 0; y < temp.size().y; y++)
			for (int x = 0; x < temp.size().x; x++)
				packet << temp.at(x, y);

		// entity lists
		encodeVec(packet, temp.troops);
		encodeVec(packet, temp.builds);
		encodeVec(packet, temp.plants);

		// region construction data
		packet << (int)temp.regions.size();
		for (const auto& rcd : temp.regions) {
			packet << rcd.res;
			packet << rcd.pos;
		};
		return packet;
	};
	/// Deserializes a map template object.
	sf::Packet& operator>>(sf::Packet& packet, Template& temp) {
		// map header
		packet >> temp.header.name;
		packet >> temp.header.auth;

		// tile data
		sf::Vector2i size = from<sf::Vector2i>(packet);
		temp.clear(size);
		for (int y = 0; y < size.y; y++)
			for (int x = 0; x < size.x; x++)
				packet >> temp.at(x, y);
		
		// entity lists
		temp.troops = decodeVec<Troop>(packet);
		temp.builds = decodeVec<Build>(packet);
		temp.plants = decodeVec<Plant>(packet);

		// region construction data
		int count = from<int>(packet);
		{
			temp.regions.reserve(count);
			for (int i = 0; i < count; i++) {
				temp.regions.push_back({
					from<RegionRes>(packet),
					from<sf::Vector2i>(packet)
				});
			};
		};
		return packet;
	};
};