#include "ui.hpp"
#include "assets.hpp"

/// Program entry.
/// @return Exit code.
int main() {

	// load languages
	if (assets::loadLanguageList())
		return 1;
	if (assets::loadLanguage("en-us.tlml"))
		return 1;

	// load assets
	assets::loadAssets();
	if (assets::error)
		return 1;
	
	// create a window
	sf::RenderWindow win(sf::VideoMode({ 1600, 900 }), "App");
	win.setVerticalSyncEnabled(true);
	win.setKeyRepeatEnabled(false);

	// create an interface
	ui::Interface itf;

	// window main loop
	std::queue<sf::Event> eventQueue;
	while (win.isOpen()) {
		// process window events
		while (const auto event = win.pollEvent()) {
			// check for window close
			if (event->is<sf::Event::Closed>()) {
				win.close();
				continue;
			};

			// pass through to interface queue
			eventQueue.push(*event);
		};

		// recalculate & update the interface
		itf.recalculate(win.getSize());
		itf.eventq(eventQueue);
		itf.update(sf::Mouse::getPosition(win));

		// redraw window contents
		win.clear(sf::Color(29, 31, 37));
		itf.draw(win);
		win.display();
	};
	return 0;
};