#pragma once

// include dependencies
#include "game/history.hpp"
#include "messages.hpp"

/// Game communication adapter.
struct Adapter {
	/// Packet type.
	/// 
	/// Stores packet data and sender adapter index.
	/// 
	/// @tparam T Packet content type.
	template <typename T> struct Packet {
		T     value; /// Packed value.
		uint32_t id; /// Player index.
	};

	/// Optional packet type.
	/// @tparam T Packet content type.
	template <typename T> using OptPacket = std::optional<Packet<T>>;

	/// Sends a move list.
	virtual void send_list(Packet<History::SpanList> list) = 0;
	/// Sends a move list from this adapter.
	void send_list(History::SpanList list);

	/// Receives a move list.
	virtual OptPacket<History::UniqList> recv_list() = 0;

	/// Sends an event.
	/// @param evt 
	virtual void send(Packet<Messages::Event> evt) = 0;
	/// Sends an event from this adapter.
	void send(const Messages::Event& evt);

	/// Receives an event.
	virtual OptPacket<Messages::Event> recv() = 0;

	/// Adapter index.
	uint32_t id {};
};