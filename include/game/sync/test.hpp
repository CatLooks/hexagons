#pragma once

// include dependencies
#include "adapter.hpp"

/// Game tester adapter.
struct TestAdapter : Adapter {
	/// Next player index response to generate.
	std::optional<size_t> next;
	/// Whether the player has sent a chat message.
	size_t chat = 0;

	/// Sends a move list.
	void send_list(Packet<History::SpanList> list) override;
	/// Receives a move list.
	OptPacket<History::UniqList> recv_list() override;

	/// Sends an event.
	void send(Packet<Event> evt) override;
	/// Receives an event.
	OptPacket<Event> recv() override;
};