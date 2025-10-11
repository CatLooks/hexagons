#include <SFML/Graphics.hpp>
#include "ui/solid.hpp"
#include "ui/layer.hpp"
#include "ui/panel.hpp"
#include <iostream>

/// Program entry.
/// @return Exit code.
int main() {
	ui::Interface interface;

	sf::Image panel_img(std::string(ASSET_PATH) + "panel.png");
	sf::Texture panel_tex(panel_img);
	ui::Layer* layer = interface.layer(&panel_tex);

	// test panel
	ui::Panel::Map map = {
		.corner_coords = { { 0, 0 }, { 8, 0 }, { 0, 8 }, { 8, 8 } },
		.edge_coords = { { 4, 0 }, { 4, 8 }, { 0, 4 }, { 8, 4 } },
		.corner_size = { 4, 4 },
		.edge_size = { 4, 4 },
		.middle = { { 4, 4 }, { 4, 4 } },
		.scale = 4
	};
	ui::Panel* p = new ui::Panel(map);
	p->bounds = { 1as - 16px, 1as - 16px, 200, 120 };
	p->color = sf::Color::Yellow;
	layer->add(p);
	{
		ui::Solid* s = new ui::Solid();
		s->color = sf::Color(0, 0, 0, 64);
		s->bounds = { 0px, 0px, 1ps, 1ps };
		p->add(s);
	};

	// test shape
	ui::Solid* panel = new ui::Solid;
	panel->bounds = { 0.5as, 0.5as, 400, 300 };
	panel->color = sf::Color::White;
	panel->onEvent([=](const ui::Event& evt) {

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
	
	ui::Solid* child = new ui::Solid;
	child->bounds = { 30, 30, 0.5ps, 0.5ps };
	child->color = sf::Color::Yellow;
	child->transparent = true;
	child->onEvent([=](const ui::Event& evt) {

		if (const auto& data = evt.get<ui::Event::KeyPress>()) {
			std::cout << "Key Press: " << (std::string)sf::Keyboard::getDescription(data->scan) << std::endl;
			std::cout << "Alt " << data->alt << " | Shift " << data->shift << " | Control " << data->control << " | System " << data->system << std::endl;
			return true;
		};
		if (const auto& data = evt.get<ui::Event::KeyRelease>()) {
			std::cout << "Key Release: " << (std::string)sf::Keyboard::getDescription(data->scan) << std::endl;
			std::cout << "Alt " << data->alt << " | Shift " << data->shift << " | Control " << data->control << " | System " << data->system << std::endl;
			return true;
		};
		if (const auto& data = evt.get<ui::Event::MousePress>()) {
			std::cout << "Mouse Press: " << (int)data->button << " at " << data->position.x << ", " << data->position.y << std::endl;
			return true;
		};
		if (const auto& data = evt.get<ui::Event::MouseRelease>()) {
			std::cout << "Mouse Release: " << (int)data->button << " at " << data->position.x << ", " << data->position.y << std::endl;
			return true;
		};
		if (const auto& data = evt.get<ui::Event::MouseEnter>()) {
			std::cout << "Mouse Enter at " << data->position.x << ", " << data->position.y << std::endl;
			return true;
		};
		if (const auto& data = evt.get<ui::Event::MouseLeave>()) {
			std::cout << "Mouse Leave at " << data->position.x << ", " << data->position.y << std::endl;
			return true;
		};
		if (const auto& data = evt.get<ui::Event::MouseWheel>()) {
			std::cout << "Mouse Wheel: " << data->delta << " at " << data->position.x << ", " << data->position.y << std::endl;
			return true;
		};
		if (const auto& data = evt.get<ui::Event::MouseMove>()) {
			//std::cout << "Mouse Move at " << data->position.x << ", " << data->position.y << std::endl;
			return true;
		};

		return false;

	});
	panel->add(child);

	ui::Element* mask = new ui::Element;
	mask->bounds = { 0px, 0px, 1ps, 0.5ps };
	mask->onEvent([=](const ui::Event& evt) {

		if (const auto& data = evt.get<ui::Event::MousePress>())
			return true;
		if (const auto& data = evt.get<ui::Event::MouseRelease>())
			return true;
		return false;

	});
	child->add(mask);

	// test text
	sf::Font font("assets/font.ttf");

	interface.setStatDrawCall([=](sf::RenderTarget& target, const ui::RenderStats& stats) {
		sf::Text text(font, std::format("{}Q | {}T | {}F", stats.quads, stats.triangles, stats.text), 24);
		text.setPosition({ 6, 0 });
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