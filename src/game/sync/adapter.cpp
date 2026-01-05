#include "game/sync/adapter.hpp"

/// Sends a move list from this adapter.
void Adapter::send_list(History::RList list) {
	send_list({ list, id });
};

/// Sends an event from this adapter.
void Adapter::send(const Event& evt) {
	send({ evt, id });
};