#include "ui.hpp"
#include "game.hpp"
#include "assets.hpp"

// map test
class Game : public ui::Element {
public:
	Map map;

	Game() {
		const int w = 7;
		const int h = 7;
		const char arr[h][w + 1] = {
			" yyyy !",
			" --wwy ",
			"bbrry-!",
			"bwrrrwg",
			"bwrrwg!",
			" b-g-g ",
			" b-gg !",
		};

		map.resize({ {}, { w, h } });
		for (int y = 0; y < h; y++) {
			for (int x = 0; x < w; x++) {
				Hex* hex = map.at({ x, y });
				if (hex == nullptr) continue;

				switch (arr[y][x]) {
				case '-':
					hex->type = Hex::Ground;
					break;
				case 'r':
					hex->type = Hex::Ground;
					hex->team = Hex::Red;
					break;
				case 'g':
					hex->type = Hex::Ground;
					hex->team = Hex::Green;
					break;
				case 'b':
					hex->type = Hex::Ground;
					hex->team = Hex::Blue;
					break;
				case 'y':
					hex->type = Hex::Ground;
					hex->team = Hex::Yellow;
					break;
				case 'w':
					hex->type = Hex::Water;
					break;

				default:
					break;
				};
			};
		};
	};

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

	// draw stats
	sf::Text drawStats = sf::Text(assets::font, "", 20);
	drawStats.setOutlineThickness(2);
	itf.statDraw([&](sf::RenderTarget& target, const ui::RenderStats& stats) {
		std::string format = std::format(
			"{}Q {}T {}B {}R",
			stats.quads,
			stats.text + 1,
			stats.batches,
			stats.inters
		);
		drawStats.setString(format);
		drawStats.setPosition({ win.getSize().x - drawStats.getLocalBounds().size.x - 6, 0 });
		target.draw(drawStats);
	});

	// game test
	auto layer = itf.layer();
	layer->add(new Game());
	layer->setArea({ 800, 450 }, { {}, {1600, 900} });

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