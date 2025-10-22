#include <SFML/Graphics.hpp>
#include "ui/solid.hpp"
#include "ui/layer.hpp"
#include "ui/panel.hpp"
#include "ui/text.hpp"
#include "ui/image.hpp"
#include "ui/anim/linear.hpp"
#include "assets.hpp"
#include "game.hpp"

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
	if (assets::error) return 1;

	// create window interface
	ui::Interface itf;
	ui::Layer* map_layer = itf.layer(&assets::tiles);

	// create game element
	GameMap* game = new GameMap();
	map_layer->add(game);

	// setup window
	sf::RenderWindow win(sf::VideoMode({ 1600, 900 }), "App");
	win.setVerticalSyncEnabled(true);
	win.setKeyRepeatEnabled(false);

	// window main loop
	while (win.isOpen()) {
		win.clear(sf::Color(34, 39, 41));

		// update mouse
		game->mouse = sf::Mouse::getPosition(win);

		// update interface
		itf.recalculate(win.getSize());
		while (const auto event = win.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				win.close();
			};
			itf.event(*event);
		};
		itf.update(sf::Mouse::getPosition(win));

		// draw interface
		itf.draw(win);
		win.display();
	};
	return 0;
};