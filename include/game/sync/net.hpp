#pragma once

// include dependencies
#include "adapter.hpp"

/// Multiplayer game adapter.
struct NetAdapter : Adapter {
	// @todo: add state
	
	/// Sends a move list.
	void send_list(Packet<History::SpanList> list) override;
	/// Receives a move list.
	OptPacket<History::UniqList> recv_list() override;

	/// Sends an event.
	void send(Packet<Messages::Event> evt) override;
	/// Receives an event.
	OptPacket<Messages::Event> recv() override;
};