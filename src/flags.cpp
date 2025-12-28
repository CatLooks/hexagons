#include "flags.hpp"

namespace flags {
	// flags
	bool debug = false;
	bool spread = false;

	/// Processes a keyboard event.
	void proc(const sf::Event::KeyPressed& evt) {
		// debug toggle
		if (evt.code == sf::Keyboard::Key::F12)
			flags::debug = !flags::debug;

		// spread toggle
		if (evt.code == sf::Keyboard::Key::F1 && evt.control)
			flags::spread = !flags::spread;
	};
};