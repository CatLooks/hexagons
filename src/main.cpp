// include SFML
#include <SFML/Graphics.hpp>

/// Program entry.
/// @return Exit code.
int main() {
	// test window
	sf::RenderWindow win(sf::VideoMode({ 1600, 900 }), "App");
	win.setVerticalSyncEnabled(true);
	while (win.isOpen()) {
		while (const auto event = win.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				win.close();
		};
		win.clear(sf::Color(29, 31, 37));
		win.display();
	};
	return 0;
};