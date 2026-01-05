#pragma once

// include dependencies
#include "game/history.hpp"
#include <variant>

/// Game communication adapter.
struct Adapter {
	/// Player event ignore.
	struct Ignore {
		size_t id; /// Player index.
	};

	/// Player selection.
	struct Select {
		size_t id; /// Player index.
	};

	/// Variant of all adapter events.
	using Event = std::variant<Ignore, Select>;

	/// Packet type.
	/// @tparam T Packet content type.
	template <typename T> struct Packet {
		T   value; /// Packed value.
		size_t id; /// Player index.
	};

	/// Optional packet type.
	/// @tparam T Packet content type.
	template <typename T> using OptPacket = std::optional<Packet<T>>;

	/// Sends a move list.
	virtual void send_list(Packet<History::RList> list) = 0;
	/// Sends a move list from this adapter.
	void send_list(History::RList list);

	/// Receives a move list.
	virtual OptPacket<History::TList> recv_list() = 0;

	/// Sends an event.
	/// @param evt 
	virtual void send(Packet<Event> evt) = 0;
	/// Sends an event from this adapter.
	void send(const Event& evt);

	/// Receives an event.
	virtual OptPacket<Event> recv() = 0;

	/// Adapter index.
	size_t id {};
};