#pragma once

// include dependencies
#include "adapter.hpp"

/// Game tester adapter.
struct TestAdapter : Adapter {
	/// Sends the move list.
	void send(const History::RList& value) override;

	/// Receives the move list.
	void recv(const History::TList& value) override;
};