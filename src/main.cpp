#include "ui.hpp"
#include "game.hpp"
#include "assets.hpp"
#include "flags.hpp"
#include "menu.hpp"
#include "networking/Net.hpp"

#include "game/serialize/map.hpp"

static void setup_test_map(Game& game) {
	Map& map = game.map;

	const int w = 18;
	const int h = 9;
	const char arr[h][w + 1] = {
		"vvv  rr     bb  rx",
		"rrvv   rrr  bbb  r",
		"vvv  rrbb   bb  rx",
		"rrvv   bbg       r",
		"vvv             rx",
		"rrvv rrr   oo    r",
		"vvv  ggg  ooo g rx",
		"rrvv ---   oo     ",
		"vvv              x"
	};

	map.empty({ w, h });

	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			Hex* hex = map.at({ x, y });
			if (hex == nullptr) continue;

			switch (arr[y][x]) {
			case '-':
				hex->type = Hex::Water;
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
			case 'v':
				hex->type = Hex::Ground;
				hex->team = Region::Purple;
				break;
			case 'o':
				hex->type = Hex::Ground;
				hex->team = Region::Orange;
				break;
			case '#':
				hex->type = Hex::Ground;
				break;
			default:
				break;
			};
		};
	};

	map.regions.enumerate(&map);

	// damage test
	{
		Troop troop;
		troop.type = Troop::Knight;
		troop.hp = troop.max_hp();

		troop.pos = { 1, 1 };
		map.setTroop(troop);
		troop.pos = { 1, 3 };
		map.setTroop(troop);

		troop.addEffect(EffectType::OffenseBoost);

		troop.pos = { 1, 5 };
		map.setTroop(troop);
		troop.pos = { 1, 7 };
		map.setTroop(troop);
	};
	{
		Troop troop;
		troop.type = Troop::Baron;
		troop.hp = troop.max_hp();

		troop.pos = { 2, 1 };
		troop.hp++;
		map.setTroop(troop);
		troop.pos = { 2, 5 };
		troop.hp--;
		map.setTroop(troop);

		troop.addEffect(EffectType::DefenseBoost);

		troop.pos = { 2, 3 };
		map.setTroop(troop);
		troop.pos = { 2, 7 };
		map.setTroop(troop);
	};

	map.at({ 1, 4 })->region()->setRes({ 999, 999, 999 });
	map.at({ 0, 1 })->region()->setRes({ 999, 999, 999 });
	map.at({ 0, 3 })->region()->setRes({ 999, 999, 999 });
	map.at({ 0, 5 })->region()->setRes({ 999, 999, 999 });
	map.at({ 0, 7 })->region()->setRes({ 999, 999, 999 });

	// region split / merge test
	{
		Troop troop;
		troop.type = Troop::Farmer;
		troop.hp = troop.max_hp();

		troop.pos = { 9, 1 };
		map.setTroop(troop);

		troop.pos = { 8, 3 };
		map.setTroop(troop);
	};

	map.at({ 9, 1 })->region()->setRes({ 999, 999, 999 });
	map.at({ 8, 3 })->region()->setRes({ 999, 999, 999 });
	map.at({ 9, 3 })->region()->setRes({ 0, 6, 9 });

	// range test
	{
		Troop troop;
		troop.type = Troop::Archer;
		troop.hp = troop.max_hp();
		troop.pos = { 5, 5 };
		map.setTroop(troop);

		troop.type = Troop::Spearman;
		troop.hp = troop.max_hp();
		troop.pos = { 6, 6 };
		map.setTroop(troop);
		troop.pos = { 7, 6 };
		map.setTroop(troop);
	};

	map.at({ 5, 5 })->region()->setRes({ 999, 999, 999 });
	map.at({ 6, 6 })->region()->setRes({ 999, 999, 999 });

	// heal test
	{
		Build build;
		build.type = Build::Tent;
		build.pos = { 13, 1 };
		map.setBuild(build);

		Troop troop;
		troop.type = Troop::Knight;
		troop.hp = 4;
		troop.pos = { 12, 0 };
		map.setTroop(troop);

		troop.type = Troop::Spearman;
		troop.hp = 1;
		troop.pos = { 13, 0 };
		map.setTroop(troop);

		troop.type = Troop::Baron;
		troop.hp = 1;
		troop.pos = { 12, 2 };
		map.setTroop(troop);

		troop.type = Troop::Archer;
		troop.hp = troop.max_hp();
		troop.pos = { 13, 2 };
		map.setTroop(troop);
	};

	map.at({ 13, 1 })->region()->setRes({ 999, 999, 999 });

	// harvest test
	{
		Troop troop;
		troop.type = Troop::Farmer;
		troop.pos = { 11, 6 };
		map.setTroop(troop);

		Plant plant;
		plant.type = Plant::Peach;
		plant.pos = { 11, 5 };
		map.setPlant(plant);

		plant.type = Plant::Sapling;
		plant.pos = { 12, 5 };
		map.setPlant(plant);

		plant.type = Plant::Berry;
		plant.pos = { 11, 7 };
		map.setPlant(plant);

		plant.type = Plant::Bush;
		plant.pos = { 12, 7 };
		map.setPlant(plant);
	};

	map.at({ 11, 6 })->region()->setRes({ 999, 0, 0 });

	// misc tests
	{
		Troop troop;
		troop.type = Troop::Lumberjack;
		troop.pos = { 12, 6 };
		map.setTroop(troop);

		Build build;
		build.type = Build::Beacon;
		build.pos = { 1, 4 };
		build.hp = build.max_hp();
		map.setBuild(build);

		Plant plant;
		plant.type = Plant::Bush;
		plant.pos = { 16, 0 };
		map.setPlant(plant);
	};
}

int main() {
	assets::lang::init();

	assets::loadAssets();
	if (assets::error) {
		fprintf(stderr, "Critical error: Failed to load game assets.\n");
		return 1;
	}

	EOSManager* eos = &EOSManager::GetInstance();

	Net net;

	ui::window.create({ 1600, 900 }, false);

	ui::Interface& itf = ui::window.interface();
	itf.clearColor(sf::Color(29, 31, 37));

	sf::Text drawStats = sf::Text(assets::font, "", 20);
	drawStats.setOutlineThickness(2);
	itf.statDraw([&](sf::RenderTarget& target, const ui::RenderStats& stats) {
		if (!flags::stats) return;

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

	GameState state(GameState::Host, new TestAdapter);

	auto game_ctx = itf.newContext();
	Game* game = nullptr;
	{
		itf.switchContext(game_ctx);

		auto layer_map = itf.layer();
		auto layer_gui = itf.layer();
		auto layer_msg = itf.layer();

		game = new Game(layer_map, layer_gui, layer_msg, &state);

		// main-branch behavior: prefilled test map + bots
		setup_test_map(*game);

		state.addPlayer({ .name = "Sus", .team = Region::Red });
		state.addPlayer({ .name = "Bot 1", .team = Region::Orange });
		state.addPlayer({ .name = "Bot 2", .team = Region::Yellow });
		state.addPlayer({ .name = "Bot 3", .team = Region::Green });
		state.addPlayer({ .name = "Bot 4", .team = Region::Aqua });
		state.addPlayer({ .name = "Bot 5", .team = Region::Blue });
		state.addPlayer({ .name = "Bot 6", .team = Region::Purple });
		state.init();

		layer_map->add(game);
		layer_gui->add(dev::Factory::game_panel(game));
	}

	MenuSystem menuSystem(itf, &game_ctx, game, net);
	itf.switchContext(menuSystem.context);

	while (ui::window.active()) {
		net.fetch();
		ui::window.events();
		ui::window.frame();
	}

	return 0;
}