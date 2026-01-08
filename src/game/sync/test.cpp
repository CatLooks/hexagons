#include "game/sync/test.hpp"

/// Sends a move list.
void TestAdapter::send_list(Packet<History::SpanList> list) {};

/// Receives a move list.
Adapter::OptPacket<History::UniqList> TestAdapter::recv_list() {
	if (next) {
		size_t idx = *next;
		next = {};

		// respond with empty move list
		return Packet<History::UniqList> { {}, idx };
	};
	return {};
};

/// Sends an event.
void TestAdapter::send(Packet<Event> evt) {
	// update player update
	if (auto* data = std::get_if<Adapter::Select>(&evt.value))
		if (data->id > 0)
			next = data->id;
};

/// Receives an event.
TestAdapter::OptPacket<TestAdapter::Event> TestAdapter::recv() {
	// no incoming events
	return {};
};