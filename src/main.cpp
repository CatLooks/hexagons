#include <SFML/Graphics.hpp>
#include "ui/solid.hpp"
#include "ui/layer.hpp"
#include "ui/panel.hpp"
#include "ui/text.hpp"
#include "ui/anim/linear.hpp"
#include "localization/parser.hpp"


sf::Font font(std::string(ASSET_PATH) + "font.ttf");

/// Program entry.
/// @return Exit code.
int main() {

	// localization test
	FILE* file;
	if (int e = fopen_s(&file, "assets/en-us.tlml", "r")) {
		char msg[256];
		strerror_s(msg, sizeof(msg), e);
		printf("error: %s\n", msg);
		return 1;
	};
	localization::State state(file);
	localization::Section root = localization::load(state);
	fclose(file);

	ui::TextSettings sets(font, 24, root);

	for (const auto& err : state.list) {
		printf("error at line %llu column %llu: ", err->line, err->column);
		err->print(stdout);
		printf("\n");
	};
	root.print(stdout);
	printf("\n");

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

		// generate solids & text
		ui::Dim y = 50px;
		for (size_t i = 0; i < 7; i++) {
			els[i] = new ui::Solid;
			layer->add(els[i]);

			els[i]->bounds = { 50px, y, 50px, 50px };
			els[i]->color = sf::Color::Red;

			{
				ui::Solid* inner = new ui::Solid;
				els[i]->add(inner);

				inner->bounds = { 10px, 10px, 1ps - 2es, 1ps - 2es };
				inner->color = sf::Color(200, 0, 0);
			}

			ui::Text* label = new ui::Text(sets, std::format("demo.{}", i));
			layer->add(label);

			label->bounds = { 650px, y, 0px, 50px };
			label->alignY = ui::Text::Center;
			label->setOutline(sf::Color::Black, 2);

			y += 80px;
		};

		// add animation callback
		ui::Solid* button = new ui::Solid;
		layer->add(button);

		button->bounds = { 50px, y, 200px, 50px };
		button->color = sf::Color(128, 0, 255);
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
		{
			ui::Text* label = new ui::Text(sets, "demo.button");
			button->add(label);

			label->setOutline(sf::Color::Black, 2.f);
			label->bounds = { 0px, 0px, 1ps, 1ps };
			label->alignX = ui::Text::Center;
			label->alignY = ui::Text::Center;
			label->shrink_to_fit = true;
		}
	}

	// test text
	interface.setStatDrawCall([=](sf::RenderTarget& target, const ui::RenderStats& stats) {
		sf::Text text(font, std::format("{}Q | {}T | {}F | {}B", stats.quads, stats.triangles, stats.text, stats.batches), 24);
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
			if (const auto* data = event->getIf<sf::Event::KeyPressed>()) {
				if (data->code == sf::Keyboard::Key::R) {
					interface.translate();
				};
			};
			interface.event(*event);
		};

		interface.update(sf::Mouse::getPosition(win));
		interface.draw(win);

		win.display();
	};
	return 0;
};