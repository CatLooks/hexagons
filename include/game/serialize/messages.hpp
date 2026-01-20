#pragma once

// include dependencies
#include "general.hpp"
#include "game/sync/messages.hpp"

namespace Serialize {
	/// Message enumeration.
	enum MessageType {
		E_Init,
		E_Select,
		E_Ignore,
		E_Chat,
		E_Count
	};

	/// Writes player description to the packet.
	/// 
	/// @param packet Target packet.
	/// @param plr Target player description.
	/// 
	/// @return Packet reference.
	sf::Packet& operator<<(sf::Packet& packet, const Messages::Player& plr);

	/// Reads player description from the packet.
	/// 
	/// @param packet Target packet.
	/// @param plr Target player description.
	/// 
	/// @return Packet reference.
	sf::Packet& operator>>(sf::Packet& packet, Messages::Player& plr);

	/// Writes an event message to the packet.
	/// 
	/// @param packet Target packet.
	/// @param vec Target event message.
	void encodeMessage(sf::Packet& packet, const Messages::Event& evt);

	/// Reads an event message from the packet.
	/// 
	/// @param packet Target packet.
	/// 
	/// @return Read event message.
	std::optional<Messages::Event> decodeMessage(sf::Packet& packet);
};

namespace NetworkProtocol {
    enum PacketType : uint8_t {
        Event = 0,
        MoveList = 1
    };
}
