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
	template <typename T> void encodeVec(sf::Packet& packet, const std::vector<T>& vec) {
		packet << (int)vec.size();
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