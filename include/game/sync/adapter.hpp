#pragma once

// include dependencies
#include "game/history.hpp"

/// I/O adapter for a single type.
/// 
/// @tparam S Sent struct type.
/// @tparam R Received struct type.
template <typename S, typename R = S> struct TypeAdapter {
	/// Sends the value.
	/// 
	/// @param value Sent struct value.
	virtual void send(const S& value) = 0;

	/// Receives the value.
	/// 
	/// @param value Received struct value.
	virtual void recv(const R& value) = 0;
};

/// Game communication adapter.
struct Adapter :
	// move sync
	TypeAdapter<History::RList, History::TList>
{};