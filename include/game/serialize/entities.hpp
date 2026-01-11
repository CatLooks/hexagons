#pragma once

// include dependencies
#include "general.hpp"
#include "game/hex.hpp"
#include "game/moves/move.hpp"

namespace Serialize {
	/// Entity state enumeration.
	enum EntityType {
		S_Empty, S_Troop, S_Build, S_Plant
	};

	/// ======== MAP OBJECTS ======== ///

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

	/// ======== ENTITIES ======== ///

	/// Serializes an entity object.
	sf::Packet& operator<<(sf::Packet& packet, const Entity& entity);
	/// Deserializes an entity object.
	sf::Packet& operator>>(sf::Packet& packet, Entity& entity);

	/// Serializes a troop object.
	sf::Packet& operator<<(sf::Packet& packet, const Troop& troop);
	/// Deserializes a troop object.
	sf::Packet& operator>>(sf::Packet& packet, Troop& troop);

	/// Serializes a building object.
	sf::Packet& operator<<(sf::Packet& packet, const Build& build);
	/// Deserializes a building object.
	sf::Packet& operator>>(sf::Packet& packet, Build& build);

	/// Serializes a plant object.
	sf::Packet& operator<<(sf::Packet& packet, const Plant& plant);
	/// Deserializes a plant object.
	sf::Packet& operator>>(sf::Packet& packet, Plant& plant);

	/// Serializes an entity state.
	sf::Packet& operator<<(sf::Packet& packet, const Moves::EntState& entity);
	/// Deserializes an entity state.
	sf::Packet& operator>>(sf::Packet& packet, Moves::EntState& entity);


};