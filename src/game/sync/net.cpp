#include "game/sync/net.hpp"
#include "game/serialize/moves.hpp"
#include "game/serialize/messages.hpp"

/// Game packet type.
enum PacketType {
	MoveList     = 0xfe,
	EventMessage = 0xc1,
};

/// Sends a move list.
void NetAdapter::send_list(Packet<History::SpanList> list) {
	// serialize move list
	sf::Packet packet;
	packet << (uint8_t)MoveList;
	packet << list.id;
	packet << (uint32_t)list.value.size();
	for (const auto& move : list.value)
		Serialize::encodeMove(packet, move.get());

	// @todo: send packet
};

/// Receives a move list.
Adapter::OptPacket<History::UniqList> NetAdapter::recv_list() {
	// @todo: add packet peeking

	// check if a packet is available
	// @note don't consume packet
	std::optional<sf::Packet> packet = {};
	if (!packet) return {};

	// switch on packet type
	switch (Serialize::from<uint8_t>(*packet)) {
		case MoveList: break;
		case EventMessage: return {};
		default:
			// @todo: consume invalid packet
			return;
	};

	// @todo: consume packet

	// deserialize move list
	auto list = Adapter::Packet<History::UniqList>{
		.id = Serialize::from<uint32_t>(*packet)
	};
	uint32_t count = Serialize::from<uint32_t>(*packet);
	for (uint32_t i = 0; i < count; i++) {
		// deserialize each move
		list.value.push_back(Serialize::decodeMove(*packet));
	};
	return list;
};

/// Sends an event.
void NetAdapter::send(Packet<Messages::Event> evt) {
	// serialize message
	sf::Packet packet;
	packet << (uint8_t)EventMessage;
	packet << evt.id;
	Serialize::encodeMessage(packet, evt.value);

	// @todo: send packet
};

/// Receives an event.
Adapter::OptPacket<Messages::Event> NetAdapter::recv() {
	// @todo: add packet peeking

	// check if a packet is available
	// @note don't consume packet
	std::optional<sf::Packet> packet = {};
	if (!packet) return {};

	// switch on packet type
	switch (Serialize::from<uint8_t>(*packet)) {
		case EventMessage: break;
		case MoveList: return {};
		default:
			// @todo: consume invalid packet
			return;
	};

	// @todo: consume packet

	// deserialize event message
	auto evt = Adapter::Packet<Messages::Event>{
		.id = Serialize::from<uint32_t>(*packet)
	};
	if (auto data = Serialize::decodeMessage(*packet)) {
		evt.value = *data;
	}
	else return {};
	return evt;
};