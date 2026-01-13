#pragma once

// include dependencies
#include "map.hpp"
#include "game/moves/move.hpp"

namespace Serialize {
	/// Entity state enumeration.
	enum EntityType {
		S_Empty, S_Troop, S_Build, S_Plant
	};

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