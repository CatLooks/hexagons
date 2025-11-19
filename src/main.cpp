#include "ui.hpp"
#include "game.hpp"
#include "assets.hpp"

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
	Game* game = new Game(layer);
	{
		Map& map = game->map;

		const int w = 14;
		const int h = 9;
		const char arr[h][w + 1] = {
			"  rrrr    b b ",
			"rrrrrr  bbbbb ",
			"rrrrr-----bbbb",
			"rrrr----bbbbbb",
			" rrrr    bbbb ",
			"  ---   ---   ",
			"   -yyyyyy-   ",
			"   yyyyyyyyyy ",
			"  y   yyyyyy  "
		};

		map.resize({ {}, { w, h } });
		{
			Troop troop;
			troop.pos = { 3, 3 };
			troop.type = Troop::Archer;
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
	}
	layer->add(game);

	auto gui = itf.layer();
	auto gp = new gameui::Panel();
	{
		gp->construct(gameui::Panel::L11);
	};
	gui->add(gp);


	// window main loop
	while (ui::window.active()) {
		ui::window.events();
		ui::window.frame();
	};
	return 0;
};