#include "ui.hpp"
#include "game.hpp"
#include "assets.hpp"



// map test
class Game : public ui::Element {
public:
	Map map;

protected:
	void drawSelf(ui::RenderBuffer& target, sf::IntRect self) const override {
		map.draw(target);
	};
};

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
	itf.clearColor(sf::Color(29, 31, 37));

	// game test
	auto layer = itf.layer();
	layer->add(new Game());

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
		itf.draw(win);
		win.display();
	};
	return 0;
};