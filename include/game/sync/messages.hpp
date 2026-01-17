#pragma once

// include dependencies
#include "game/template.hpp"
#include <string>
#include <variant>

/// Game adapter messages.
namespace Messages {
	/// Player description.
	struct Player {
		std::string  name; /// Player name.
		Region::Team team; /// Player team.
	};

	/// Game initialization.
	struct Init {
		/// Map template.
		Template temp;

		/// Player list.
		std::vector<Player> players;
	};

	/// Player move list ignore.
	struct Ignore {
		uint32_t id  = 0; /// Ignore target.
		uint32_t now = 0; /// Player currently making a move.
	};

	/// Player selection.
	struct Select {
		uint32_t id = 0; /// Player index.
		bool   turn = 0; /// Whether next turn began.
	};

	/// Chat message.
	struct Chat {
		std::string text; /// Message text.
	};

	/// Variant of all adapter events.
	using Event = std::variant<
		Init,
		Ignore,
		Select,
		Chat
	>;
};