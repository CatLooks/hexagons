#pragma once

// include dependencies
#include "adapter.hpp"

/// Game tester adapter.
struct TestAdapter : Adapter {
	/// Next player index response to generate.
	std::optional<size_t> next;

	/// Sends a move list.
	void send_list(Packet<History::RList> list) override;
	/// Receives a move list.
	OptPacket<History::TList> recv_list() override;

	/// Sends an event.
	void send(Packet<Event> evt) override;
	/// Receives an event.
	OptPacket<Event> recv() override;
};