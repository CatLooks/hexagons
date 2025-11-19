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
		const int h = 7;
		const char arr[h][w + 1] = {
			"------------- ",
			"rrrrrrrrrrrrrr",
			"rrrrrrrrrrrrr ",
			"gggggggggggggg",
			"ggggggggggggg ",
			"bbbbbbbbbbbbbb",
			"bbbbbbbbbbbbb ",
		};

		map.resize({ {}, { w, h } });
		{
			int pos = 0;
			for (int idx = 0; idx < Troop::Count; idx++) {
				if (idx & 1) continue;

				Troop troop;
				troop.pos = { pos += 2, 1 };
				troop.type = static_cast<Troop::Type>(idx);
				troop.hp = 1;
				map.setTroop(troop);
			};
		}
		{
			int pos = 0;
			for (int idx = 0; idx < Build::Count; idx++) {
				if (idx == Build::Farm || idx == Build::Mine || idx == Build::Fort) continue;

				Build build;
				build.pos = { pos += 2, 3 };
				build.type = static_cast<Build::Type>(idx);
				map.setBuild(build);
			};
		}
		{
			int pos = 0;
			for (int idx = 0; idx < Plant::Count; idx++) {
				if (idx == Plant::Berry || idx == Plant::Sapling || idx == Plant::Peach || idx == Plant::Grave) continue;

				Plant plant;
				plant.pos = { pos += 2, 5 };
				plant.type = static_cast<Plant::Type>(idx);
				map.setPlant(plant);
			};
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
	gui->add(gp);

	// window main loop
	while (ui::window.active()) {
		ui::window.events();
		ui::window.frame();
	};
	return 0;
};