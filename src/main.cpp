#include <SFML/Graphics.hpp>
#include "ui/solid.hpp"
#include "ui/layer.hpp"
#include "ui/panel.hpp"
#include "ui/text.hpp"
#include "ui/image.hpp"
#include "ui/anim/linear.hpp"
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

	// window main loop
	while (win.isOpen()) {
		// process events
		while (const auto event = win.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				win.close();
			};
		};

		// render new frame
		win.clear(sf::Color(29, 31, 37));
		win.display();
	};
	return 0;
};