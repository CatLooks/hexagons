#include "game/serialize/general.hpp"

namespace Serialize {
	/// Writes a vector to the packet.
	sf::Packet& operator<<(sf::Packet& packet, sf::Vector2i vec) {
		packet << vec.x;
		packet << vec.y;
		return packet;
	};

	/// Reads a vector from the packet.
	sf::Packet& operator>>(sf::Packet& packet, sf::Vector2i& vec) {
		packet >> vec.x;
		packet >> vec.y;
		return packet;
	};
};