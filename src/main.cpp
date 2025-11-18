#include "ui.hpp"
#include "game.hpp"
#include "assets.hpp"

// map test
class Game : public ui::Element {
public:
	Map map;
	ui::Layer* layer;
	ui::Camera cam;

	Game(ui::Layer* layer): layer(layer), cam(layer, &map.camera, 17.f / 16) {
		cam.minZoom = 0.5f;
		cam.maxZoom = 2.0f;
		layer->infinite = true;

		const int w = 15;
		const int h = 7;
		const char arr[h][w + 1] = {
			" yyyy !  yyyy !",
			" --wwy   --wwy ",
			"bbrry-! bbrry-!",
			"bwrrrwg bwrrrwg",
			"bwrrwg! bwrrwg!",
			" b-g-g   b-g-g ",
			" b-gg !  b-gg !"
		};

		map.resize({ {}, { w, h } });
		{
			Troop troop;
			troop.pos = { 3, 3 };
			troop.type = Troop::Farmer;
			troop.hp = 1;
			map.setTroop(std::move(troop));
		}
		{
			Build build;
			build.pos = { 2, 3 };
			build.type = Build::Tower;
			map.setBuild(std::move(build));
		}
		{
			Plant plant;
			plant.pos = { 4, 3 };
			plant.type = Plant::Peach;
			map.setPlant(std::move(plant));
		}

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

		layer->onEvent([=](const ui::Event& evt) {
			if (auto data = evt.get<ui::Event::MouseWheel>()) {
				/*cam.scroll(
					-data->delta,
					ui::window.mouse(),
					ui::window.size()
				);*/
				// uncomment after fixing
				return true;
			};

			return false;
		});

		layer->onUpdate([=](const sf::Time& _) {
			layer->setArea(ui::DimVector{ 1es, 1es } * cam.zoom(), { 0px, 0px, 1ps, 1ps });

			sf::Vector2i offset;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) offset.y--;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) offset.y++;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) offset.x--;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) offset.x++;
			map.camera += offset * 2;

			cam.pan(
				sf::Mouse::isButtonPressed(sf::Mouse::Button::Right),
				ui::window.mouse(), ui::window.size()
			);
		});
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

	// create window
	ui::window.create({ 1600, 900 });

	// create an interface
	ui::Interface& itf = ui::window.interface();
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
		drawStats.setPosition({ ui::window.size().x - drawStats.getLocalBounds().size.x - 4, 0 });
		target.draw(drawStats);
	});

	// game test
	auto layer = itf.layer();
	layer->add(new Game(layer));

	// window main loop
	while (ui::window.active()) {
		ui::window.events();
		ui::window.frame();
	};
	return 0;
};