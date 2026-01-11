#pragma once

// include dependencies
#include "adapter.hpp"

/// Game tester adapter.
struct TestAdapter : Adapter {
	/// Next player index response to generate.
	std::optional<uint32_t> next;
	/// Player count.
	uint32_t count = 0;
	/// Whether the player has sent a chat message.
	uint32_t chat = 0;

	/// Sends a move list.
	void send_list(Packet<History::SpanList> list) override;
	/// Receives a move list.
	OptPacket<History::UniqList> recv_list() override;

	/// Sends an event.
	void send(Packet<Messages::Event> evt) override;
	/// Receives an event.
	OptPacket<Messages::Event> recv() override;
};