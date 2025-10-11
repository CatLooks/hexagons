#include <SFML/Graphics.hpp>
#include "ui/panel.hpp"
#include "ui/layer.hpp"

/// Program entry.
/// @return Exit code.
int main() {
	ui::Interface interface;
	ui::Layer* layer = interface.layer(nullptr);

	// test shape
	ui::Panel* panel = new ui::Panel;
	panel->bounds = { 0.5as, 0.5as, 400, 300 };
	panel->color = sf::Color::White;
	panel->onEvent([=](ui::Element& _, const ui::Event& evt) {

		if (evt.is<ui::Event::MouseEnter>()) {
			panel->color = sf::Color::Red;
			return true;
		};
		if (evt.is<ui::Event::MouseLeave>()) {
			panel->color = sf::Color::White;
			return true;
		};

		return false;

	});
	layer->add(panel);
	
	ui::Panel* child = new ui::Panel;
	child->bounds = { 30, 30, 0.5ps, 0.5ps };
	child->color = sf::Color::Yellow;
	child->transparent = false;
	child->onEvent([=](ui::Element& _, const ui::Event& evt) {

		if (const auto& data = evt.get<ui::Event::KeyPress>()) {
			printf("Press: %d\n", data->key);

			return true;
		};

		return false;

	});
	panel->add(child);

	// test text
	sf::Font font("assets/font.ttf");

	interface.setStatDrawCall([=](sf::RenderTarget& target, const ui::RenderStats& stats) {
		sf::Text text(font, std::format("{}Q | {}T | {}F", stats.quads, stats.triangles, stats.text), 24);
		text.setOutlineThickness(2);
		target.draw(text);
	});

	// test window
	sf::RenderWindow win(sf::VideoMode({ 1600, 900 }), "App");
	win.setVerticalSyncEnabled(true);
	win.setKeyRepeatEnabled(false);

	while (win.isOpen()) {
		win.clear(sf::Color(29, 31, 37));

		interface.recalculate(win.getSize());
		while (const auto event = win.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				win.close();
			};
			interface.event(*event);
		};

		interface.update(sf::Mouse::getPosition(win));
		interface.draw(win);

		win.display();
	};
	return 0;
};