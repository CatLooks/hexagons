#include "game/serialize/general.hpp"

namespace Serialize {
	/// Writes a vector to the packet.
	sf::Packet& operator<<(sf::Packet& packet, sf::Vector2i vec) {
		packet << (int16_t)vec.x;
		packet << (int16_t)vec.y;
		return packet;
	};

	/// Reads a vector from the packet.
	sf::Packet& operator>>(sf::Packet& packet, sf::Vector2i& vec) {
		packet >> (int16_t&)vec.x;
		packet >> (int16_t&)vec.y;
		return packet;
	};
};