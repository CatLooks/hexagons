#include "game/sync/test.hpp"
#include "game/serialize/moves.hpp"
#include "game/serialize/messages.hpp"

/// Prints packet bytes.
static void print_packet(const sf::Packet& packet) {
	printf("Packet (Size = %llu):\n", packet.getDataSize());
	auto data = reinterpret_cast<const uint8_t*>(packet.getData());
	for (size_t i = 0; i < packet.getDataSize(); i++) {
		printf("%02x ", data[i]);
		if (!((i + 1) & 0x1f)) printf("\n");
	};
	if (packet.getDataSize() & 0x1f) printf("\n");
	printf("\n\n");
};

/// Sends a move list.
void TestAdapter::send_list(Packet<History::SpanList> list) {
	/*printf("Move list from %u:\n", list.id);

	// serialization test
	sf::Packet packet;
	packet << list.id;
	packet << (uint8_t)0xFE;
	packet << (int)list.value.size();
	for (const auto& move : list.value)
		Serialize::encodeMove(packet, move.get());

	print_packet(packet);*/
};

/// Receives a move list.
Adapter::OptPacket<History::UniqList> TestAdapter::recv_list() {
	if (next) {
		uint32_t idx = *next;
		next = {};

		// respond with empty move list
		return Packet<History::UniqList> { {}, idx };
	};
	return {};
};

/// Sends an event.
void TestAdapter::send(Packet<Messages::Event> evt) {
	/*{
		printf("Event from %u:\n", evt.id);

		// serialization test
		sf::Packet packet;
		packet << evt.id;
		packet << (uint8_t)0xFF;
		Serialize::encodeMessage(packet, evt.value);

		print_packet(packet);
	};*/

	// game initialization
	if (auto* data = std::get_if<Messages::Init>(&evt.value))
		count = (uint32_t)data->players.size();

	// player selection
	if (auto* data = std::get_if<Messages::Select>(&evt.value))
		if (data->id > 0)
			next = data->id;

	// chat message
	if (auto* data = std::get_if<Messages::Chat>(&evt.value))
		if (evt.id == 0)
			chat = 1;
};

/// Receives an event.
Adapter::OptPacket<Messages::Event> TestAdapter::recv() {
	// create a response to chat message
	if (chat) {
		auto pack = Packet<Messages::Event> { Messages::Chat{ .text = "stfu" }, chat };
		chat = (chat + 1) % count;
		return pack;
	};

	// no incoming events
	return {};
};