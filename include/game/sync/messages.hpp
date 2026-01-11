#pragma once

// include dependencies
#include <string>
#include <variant>

/// Game adapter messages.
namespace Messages {
	/// Player event ignore.
	struct Ignore {
		size_t id; /// Player index.
	};

	/// Player selection.
	struct Select {
		size_t id; /// Player index.
	};

	/// Chat message.
	struct Chat {
		std::string text; /// Message text.
	};

	/// Variant of all adapter events.
	using Event = std::variant<
		Ignore,
		Select,
		Chat
	>;
};