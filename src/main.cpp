#include "ui.hpp"
#include "game.hpp"
#include "assets.hpp"
#include <pool>

struct Test {
	int x;

	Test(int i): x(i) {};
	~Test() {  };
};

/// Program entry.
/// @return Exit code.
int main() {
	Pool<Test> pool;
	{
		std::vector<Pool<Test>::Item> items;

		{
			auto it = pool.add(1);
			printf("=== %d\n", it->x);
		};
		{
			auto ita = pool.add(2);
			auto itb = pool.add(3);
			printf("=== %d @ %llu\n", ita->x, ita.index());
			printf("=== %d @ %llu\n", itb->x, itb.index());
		};
		{
			items.push_back(pool.add(4));
			auto itc = pool.add(5);
			items.push_back(pool.add(6));

			printf("=== %d @ %llu\n", items[0]->x, items[0].index());
			printf("=== %d @ %llu\n", itc->x, itc.index());
			printf("=== %d @ %llu\n", items[1]->x, items[1].index());
		};
		{
			printf("=== %d @ %llu\n", items[0]->x, items[0].index());
			printf("=== %d @ %llu\n", items[1]->x, items[1].index());
		};
	};
	return 0;

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
			"yyyrrrrrrrrrrr",
			"rrrrrrrrrrrrr ",
			"gggggggggggggg",
			"ggggggggggggg ",
			"bbbbbbbbbbbbbb",
			"bbbbbbbbbbbbb ",
		};

		map.resize({ {}, { w, h } });
		{
			sf::Vector2i pos;
			for (int idx = 0; idx < Troop::Count; idx++) {
				for (int hp = 0; hp <= Values::troop_hp[idx]; hp++) {
					Troop troop;
					troop.pos = pos;
					troop.type = static_cast<Troop::Type>(idx);
					troop.hp = hp;
					map.setTroop(troop);

					pos.x++;
				};
				pos.x = 0;
				pos.y++;
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