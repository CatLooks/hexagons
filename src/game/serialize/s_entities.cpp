#include "game/serialize/entities.hpp"

namespace Serialize {
	/// ======== MAP OBJECTS ======== ///

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

	/// ======== ENTITIES ======== ///

	/// Serializes an entity object.
	sf::Packet& operator<<(sf::Packet& packet, const Entity& entity) {
		// entity position
		packet << entity.pos;

		// entity health
		packet << entity.hp;

		// entity timers
		for (int i = 0; i < 4; i++)
			packet << entity.timers[i];

		// entity effects
		packet << (uint8_t)entity.effectList().size();
		for (EffectType eff : entity.effectList())
			packet << static_cast<uint8_t>(eff);
		return packet;
	};
	/// Deserializes an entity object.
	sf::Packet& operator>>(sf::Packet& packet, Entity& entity) {
		// entity position
		packet >> entity.pos;

		// entity health
		packet >> entity.hp;

		// entity timers
		for (int i = 0; i < 4; i++)
			packet >> entity.timers[i];

		// entity effects
		int count = from<uint8_t>(packet);
		while (count-- > 0) {
			auto byte = from<uint8_t>(packet);
			if (byte >= static_cast<int>(EffectType::Count))
				byte = 0;
			entity.addEffect(static_cast<EffectType>(byte));
		};
		return packet;
	};

	/// Serializes a troop object.
	sf::Packet& operator<<(sf::Packet& packet, const Troop& troop) {
		packet << (const Entity&)troop;
		packet << (uint8_t)troop.type;
		return packet;
	};
	/// Deserializes a troop object.
	sf::Packet& operator>>(sf::Packet& packet, Troop& troop) {
		packet >> (Entity&)troop;
		auto byte = from<uint8_t>(packet);
		if (byte >= Troop::Count) byte = 0;
		troop.type = static_cast<Troop::Type>(byte);
		return packet;
	};

	/// Serializes a building object.
	sf::Packet& operator<<(sf::Packet& packet, const Build& build) {
		packet << (const Entity&)build;
		packet << (uint8_t)build.type;
		return packet;
	};
	/// Deserializes a building object.
	sf::Packet& operator>>(sf::Packet& packet, Build& build) {
		packet >> (Entity&)build;
		auto byte = from<uint8_t>(packet);
		if (byte >= Build::Count) byte = 0;
		build.type = static_cast<Build::Type>(byte);
		return packet;
	};

	/// Serializes a plant object.
	sf::Packet& operator<<(sf::Packet& packet, const Plant& plant) {
		packet << (const Entity&)plant;
		packet << (uint8_t)plant.type;
		return packet;
	};
	/// Deserializes a plant object.
	sf::Packet& operator>>(sf::Packet& packet, Plant& plant) {
		packet >> (Entity&)plant;
		auto byte = from<uint8_t>(packet);
		if (byte >= Plant::Count) byte = 0;
		plant.type = static_cast<Plant::Type>(byte);
		return packet;
	};

	/// Serializes an entity state.
	sf::Packet& operator<<(sf::Packet& packet, const Moves::EntState& entity) {
		if (auto* data = std::get_if<Moves::Empty>(&entity)) {
			packet << (uint8_t)S_Empty;
			packet << data->pos;
		};
		if (auto* data = std::get_if<Troop>(&entity)) {
			packet << (uint8_t)S_Troop;
			packet << *data;
		};
		if (auto* data = std::get_if<Build>(&entity)) {
			packet << (uint8_t)S_Build;
			packet << *data;
		};
		if (auto* data = std::get_if<Plant>(&entity)) {
			packet << (uint8_t)S_Plant;
			packet << *data;
		};
		return packet;
	};
	/// Deserializes an entity state.
	sf::Packet& operator>>(sf::Packet& packet, Moves::EntState& entity) {
		// get type
		auto type = from<uint8_t>(packet);

		// get state data
		if (type == S_Troop)
			entity = from<Troop>(packet);
		else if (type == S_Build)
			entity = from<Build>(packet);
		else if (type == S_Plant)
			entity = from<Plant>(packet);
		else {
			Moves::Empty value;
			packet >> value.pos;
			entity = value;
		};
		return packet;
	};
};