#include "game/sync/ai.hpp"
#include "game/bot_ai.hpp"

/// Constructs a bot adapter.
BotAdapter::BotAdapter(float difficulty):
	difficulty(difficulty) {};

/// Sends a move list.
void BotAdapter::send_list(Packet<History::SpanList> list) {};

/// Receives a move list.
Adapter::OptPacket<History::UniqList> BotAdapter::recv_list() {
	// check if current player is a bot
	if (next) {
		uint32_t idx = *next;
		next = {};

		// ignore if player's turn
		if (idx == 0) return {};

		// check if map is attached
		if (!map) {
			fprintf(stderr, "[ERROR] Map is not attached to bot adapter.\n");
			return {};
		};

		// return generated move list
		auto moves = ai::generate(*map, list[idx].team, difficulty);
		return Packet<History::UniqList> { .value = std::move(moves), .id = idx };
	};

	// no move lists
	return {};
};

/// Sends an event.
void BotAdapter::send(Packet<Messages::Event> evt) {
	// game initialization
	if (auto* data = std::get_if<Messages::Init>(&evt.value))
		list = data->players;

	// player selection
	if (auto* data = std::get_if<Messages::Select>(&evt.value))
		if (data->id > 0)
			next = data->id;
};

/// Receives an event.
Adapter::OptPacket<Messages::Event> BotAdapter::recv() {
	// no events
	return {};
};