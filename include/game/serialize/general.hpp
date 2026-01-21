#pragma once

// include dependencies
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "game/moves/move.hpp" // zapewnia Moves::EntState (alias na std::variant)

// forward declarations typów u¿ywanych w przeci¹¿eniach
struct Entity;
struct Troop;
struct Build;
struct Plant;
struct RegionRes;
struct RegionVar;
class Template;
namespace Messages { struct Player; }

/// Serialization helper functions.
namespace Serialize {
	// forward declarations operatorów – musz¹ byæ widoczne przed szablonem from<T>
	sf::Packet& operator<<(sf::Packet& packet, sf::Vector2i vec);
	sf::Packet& operator>>(sf::Packet& packet, sf::Vector2i& vec);

	sf::Packet& operator<<(sf::Packet& packet, const Entity& entity);
	sf::Packet& operator>>(sf::Packet& packet, Entity& entity);

	sf::Packet& operator<<(sf::Packet& packet, const Troop& troop);
	sf::Packet& operator>>(sf::Packet& packet, Troop& troop);

	sf::Packet& operator<<(sf::Packet& packet, const Build& build);
	sf::Packet& operator>>(sf::Packet& packet, Build& build);

	sf::Packet& operator<<(sf::Packet& packet, const Plant& plant);
	sf::Packet& operator>>(sf::Packet& packet, Plant& plant);

	sf::Packet& operator<<(sf::Packet& packet, const RegionRes& res);
	sf::Packet& operator>>(sf::Packet& packet, RegionRes& res);

	sf::Packet& operator<<(sf::Packet& packet, const RegionVar& var);
	sf::Packet& operator>>(sf::Packet& packet, RegionVar& var);

	sf::Packet& operator<<(sf::Packet& packet, const Template& temp);
	sf::Packet& operator>>(sf::Packet& packet, Template& temp);

	sf::Packet& operator<<(sf::Packet& packet, const Messages::Player& plr);
	sf::Packet& operator>>(sf::Packet& packet, Messages::Player& plr);

	sf::Packet& operator<<(sf::Packet& packet, const Moves::EntState& entity);
	sf::Packet& operator>>(sf::Packet& packet, Moves::EntState& entity);

	/// Reads a type from the packet.
	///
	/// @tparam T Read type.
	///
	/// @param packet Target packet.
	///
	/// @return Type value.
	template <typename T> T from(sf::Packet& packet) {
		T var {};
		using Serialize::operator>>; // widoczne przeci¹¿enia w Serialize
		packet >> var;
		return var;
	};

	/// Writes a vector list to the packet.
	/// 
	/// @tparam T Vector item type.
	/// 
	/// @param packet Target packet.
	/// @param vec Target vector.
	template <typename T> void encodeVec(sf::Packet& packet, const std::vector<T>& vec) {
		packet << static_cast<int>(vec.size());
		for (const T& item : vec)
			packet << item;
	};

	/// Reads a vector list from the packet.
	/// 
	/// @tparam T Vector item type.
	/// 
	/// @param packet Target packet.
	/// 
	/// @return Read vector.
	template <typename T> std::vector<T> decodeVec(sf::Packet& packet) {
		int count = from<int>(packet);
		std::vector<T> vec;
		{
			vec.reserve(count);
			for (int i = 0; i < count; i++)
				vec.push_back(from<T>(packet));
		};
		return vec;
	};

	/// Writes a 2d vector to the packet.
	/// 
	/// @param packet Target packet.
	/// @param vec Target vector.
	/// 
	/// @return Packet reference.
	sf::Packet& operator<<(sf::Packet& packet, sf::Vector2i vec);

	/// Reads a 2d vector from the packet.
	/// 
	/// @param packet Target packet.
	/// @param vec Target vector.
	/// 
	/// @return Packet reference.
	sf::Packet& operator>>(sf::Packet& packet, sf::Vector2i& vec);
};
