#pragma once

// include dependencies
#include <SFML/Window/Event.hpp>

/// Global flags namespace.
namespace flags {
	/// Whether the debug mode is enabled.
	extern bool debug;

	/// Whether to show spread index for every tile.
	extern bool spread;

	/// Processes a keyboard event.
	/// 
	/// @param evt Event data.
	void proc(const sf::Event::KeyPressed& evt);
};