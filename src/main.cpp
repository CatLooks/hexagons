#include <SFML/Graphics.hpp>
#include "ui/solid.hpp"
#include "ui/layer.hpp"
#include "ui/panel.hpp"
#include "ui/anim/linear.hpp"
#include <iostream>

/// Program entry.
/// @return Exit code.
int main() {
	ui::Interface interface;
	ui::Layer* layer = interface.layer(nullptr);

	{
		ui::Easing list[7] {
			ui::Easings::linear,
			ui::Easings::quadIn,
			ui::Easings::quadOut,
			ui::Easings::quad,
			ui::Easings::sineIn,
			ui::Easings::sineOut,
			ui::Easings::sine
		};
		ui::Solid* els[7] { nullptr };

		// generate solids
		ui::Dim y = 50px;
		for (size_t i = 0; i < 7; i++) {
			els[i] = new ui::Solid;
			layer->add(els[i]);

			els[i]->bounds = { 50px, y, 50px, 50px };
			els[i]->color = sf::Color::Red;
			y += 80px;
		};

		// add animation callback
		ui::Solid* button = new ui::Solid;
		layer->add(button);

		button->bounds = { 50px, y, 200px, 50px };
		button->color = sf::Color::Yellow;
		button->onEvent([=](const ui::Event& evt) {
			if (const auto data = evt.get<ui::Event::MousePress>()) {
				if (data->button == sf::Mouse::Button::Left) {
					for (size_t i = 0; i < 7; i++) {
						ui::Anim* anim = new ui::AnimDim(&els[i]->position().x, 50px, 550px, sf::seconds(1));
						anim->setEasing(list[i]);
						button->push(anim);
					};
				};
				return true;
			};

			return false;
		});
	}

	// test text
	sf::Font font(std::string(ASSET_PATH) + "font.ttf");
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