#pragma once

// include dependencies
#include <SFML/Network/Packet.hpp>
#include <SFML/System/Vector2.hpp>

/// Serialization helper functions.
namespace Serialize {
	/// Reads a type from the packet.
	/// 
	/// @tparam T Read type.
	/// 
	/// @param packet Target packet.
	/// 
	/// @return Type value.
	template <typename T> T from(sf::Packet& packet) {
		T var {};
		packet >> var;
		return var;
	};

	/// Writes a vector list to the packet.
	/// 
	/// @tparam T Vector item type.
	/// 
	/// @param packet Target packet.
	/// @param vec Target vector.
	/// 
	/// @return Packet reference.
	template <typename T>
	sf::Packet& operator<<(sf::Packet& packet, const std::vector<T>& vec) {
		packet << (int)vec.size();
		for (const auto& item : vec)
			packet << item;
		return packet;
	};

	/// Reads a vector list from the packet.
	/// 
	/// @tparam T Vector item type.
	/// 
	/// @param packet Target packet.
	/// @param vec Target vector.
	/// 
	/// @return Packet reference.
	template <typename T>
	sf::Packet& operator>>(sf::Packet& packet, std::vector<T>& vec) {
		int count = from<int>(packet);
		for (int i = 0; i < count; i++)
			vec.push_back(from<T>(packet));
		return packet;
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