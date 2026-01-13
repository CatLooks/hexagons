#include "game/sync/test.hpp"
#include "game/serialize/messages.hpp"

/// Sends a move list.
void TestAdapter::send_list(Packet<History::SpanList> list) {};

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