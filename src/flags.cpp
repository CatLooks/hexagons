#include "flags.hpp"

namespace flags {
	// flags
	bool stats      = true;
	bool debug      = false;
	bool any_region = false;
	bool spread     = false;

	/// Processes a keyboard event.
	void proc(const sf::Event::KeyPressed& evt) {
		// debug toggle
		if (evt.code == sf::Keyboard::Key::F12 && evt.control)
			flags::debug = !flags::debug;

		// stats toggle
		if (evt.code == sf::Keyboard::Key::F10)
			flags::stats = !flags::stats;

		// debug flags
		if (flags::debug || true) {
			// any region toggle
			if (evt.code == sf::Keyboard::Key::F1 && evt.control)
				flags::any_region = !flags::any_region;

			// spread toggle
			if (evt.code == sf::Keyboard::Key::F2 && evt.control)
				flags::spread = !flags::spread;
		};
	};
};