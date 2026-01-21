#pragma once

// include dependencies
#include "adapter.hpp"

/// AI game adapter.
struct BotAdapter : Adapter {
	/// Map reference.
	Map* map = nullptr;
	/// Current player index.
	std::optional<uint32_t> next = 0;
	/// Bot difficulty coefficient.
	float difficulty = 1.f;
	/// Player list.
	std::vector<Messages::Player> list;

	/// Constructs a bot adapter.
	/// 
	/// @param map Map reference.
	/// @param difficulty Difficulty coefficient (from 0 to 1).
	BotAdapter(float difficulty);

	/// Sends a move list.
	void send_list(Packet<History::SpanList> list) override;
	/// Receives a move list.
	OptPacket<History::UniqList> recv_list() override;

	/// Sends an event.
	void send(Packet<Messages::Event> evt) override;
	/// Receives an event.
	OptPacket<Messages::Event> recv() override;
};