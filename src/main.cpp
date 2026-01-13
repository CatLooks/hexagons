#include "ui.hpp"
#include "game.hpp"
#include "assets.hpp"
#include "menu.hpp"

#include <iostream>
/// Program entry.
/// @return Exit code.
int main() {
	// load languages
	if (assets::loadLanguageList())
		return 1;
	if (assets::loadLanguage("en-us.tlml"))
		return 1;

	assets::lang::init();

	// load assets
	assets::loadAssets();
	if (assets::error)
		return 1;

	// create window
	ui::window.create({ 1600, 900 }, false);

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


	//creation of game context
	auto game_ctx = itf.newContext();
	Game* game;
	{
		itf.switchContext(game_ctx);

		// game test
		auto layer_map = itf.layer();
		auto layer_gui = itf.layer();
		game = new Game(layer_map, layer_gui);
		{
			Map& map = game->map;

			const int w = 14;
			const int h = 7;
			const char arr[h][w + 1] = {
				"------------- ",
				"rrrrrrrwyywrrr",
				"rrrrrrwyywrrr ",
				"ggggggwyywgggg",
				"ggrrgwyywgggg ",
				"bbrrbwyywbbbbb",
				"bbbbwyywbbbbb ",
			};

			map.empty({ w, h });
			{
				Troop troop;
				troop.pos = { 1, 1 };
				troop.type = Troop::Knight;
				troop.hp = 5;
				map.setTroop(troop);

				Build build;
				build.pos = { 3, 1 };
				build.type = Build::Tower;
				map.setBuild(build);

				Plant plant;
				plant.pos = { 5, 1 };
				plant.type = Plant::Peach;
				map.setPlant(plant);
			};

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
						hex->team = Region::Red;
						break;
					case 'g':
						hex->type = Hex::Ground;
						hex->team = Region::Green;
						break;
					case 'b':
						hex->type = Hex::Ground;
						hex->team = Region::Blue;
						break;
					case 'y':
						hex->type = Hex::Ground;
						hex->team = Region::Yellow;
						break;
					case 'w':
						hex->type = Hex::Water;
						break;

					default:
						break;
					};
				};
			};
			map.regions.enumerate(&map);
		};
		layer_map->add(game);
	}


	MenuSystem menuSystem(itf, &game_ctx, game);
	itf.switchContext(menuSystem.context);



	// window main loop
	while (ui::window.active()) {
		ui::window.events();
		ui::window.frame();
	};
	return 0;
};