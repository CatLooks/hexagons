#include "flags.hpp"

namespace flags {
	// flags
	bool debug = false;
	bool any_region = true;
	bool spread = false;

	/// Processes a keyboard event.
	void proc(const sf::Event::KeyPressed& evt) {
		// debug toggle
		if (evt.code == sf::Keyboard::Key::F12 && evt.control)
			flags::debug = !flags::debug;

		// debug flags
		if (flags::debug) {
			// any region toggle
			if (evt.code == sf::Keyboard::Key::F1 && evt.control)
				flags::any_region = !flags::any_region;

			// spread toggle
			if (evt.code == sf::Keyboard::Key::F2 && evt.control)
				flags::spread = !flags::spread;
		};
	};
};