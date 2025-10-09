// include SFML
#include <SFML/Graphics.hpp>

sf::RenderWindow win(sf::VideoMode({ 1600, 900 }), "App");

#include "ui/panel.hpp"
#include "ui/layer.hpp"

/// Program entry.
/// @return Exit code.
int main() {
	ui::Interface UI;
	ui::Layer* layer = UI.newLayer();

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
	
	ui::Panel* child = new ui::Panel;
	child->bounds = { 30, 30, 0.5ps, 0.5ps };
	child->color = sf::Color::Yellow;
	child->transparent = false;
	panel->add(child);

	layer->add(panel);

	// test window
	win.setVerticalSyncEnabled(true);
	while (win.isOpen()) {
		win.clear(sf::Color(29, 31, 37));

		UI.recalculate(win.getSize());
		while (const auto event = win.pollEvent()) {
			if (event->is<sf::Event::Closed>())
				win.close();
			UI.event(*event);
		};

		UI.update(win, 0);
		UI.draw();

		win.display();
	};
	return 0;
};