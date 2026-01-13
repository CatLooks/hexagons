#include "ui.hpp"
#include "game.hpp"
#include "assets.hpp"
#include "flags.hpp"

#include "game/serialize/map.hpp"

/// Testing phase.
/// 
/// @return Whether to exit the app after testing.
static bool test() {
	return false;
};

/// Program entry.
/// @return Exit code.
int main() {
	// testing phase
	if (test()) return 1;
	
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
	ui::window.create({ 1600, 900 }, false);

	// create an interface
	ui::Interface& itf = ui::window.interface();
	itf.clearColor(sf::Color(29, 31, 37));

	// draw stats
	sf::Text drawStats = sf::Text(assets::font, "", 20);
	drawStats.setOutlineThickness(2);
	itf.statDraw([&](sf::RenderTarget& target, const ui::RenderStats& stats) {
		// ignore if not needed
		if (!flags::stats) return;

		// draw stat string
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
	Game* game;

	// game test
	auto game_ctx = itf.newContext();
	{
		auto layer_map = itf.layer();
		auto layer_gui = itf.layer();
		auto layer_msg = itf.layer();

		game = new Game(layer_map, layer_gui, layer_msg, &state);

		state.addPlayer({ .name = "Sus", .team = Region::Red });
		state.addPlayer({ .name = "Bot 1", .team = Region::Orange });
		state.addPlayer({ .name = "Bot 2", .team = Region::Yellow });
		state.addPlayer({ .name = "Bot 3", .team = Region::Green });
		state.addPlayer({ .name = "Bot 4", .team = Region::Aqua });
		state.addPlayer({ .name = "Bot 5", .team = Region::Blue });
		state.addPlayer({ .name = "Bot 6", .team = Region::Purple });
		state.init();

		{
			Map& map = game->map;

			const int w = 24;
			const int h = 7;
			const char arr[h][w + 1] = {
				"------   ggggyyy   oooo ",
				"--rr---  ggggyyyy  vvvvv",
				"---rbb   gggyyyy   vvvv ",
				"rrrrbb#  gggg-yyy  vvvvv",
				"---rrb   ggggyyy   vvvv ",
				"--rr---  ggggyyyy  vvvvv",
				"------   ggggyyy   oooo "
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

			{
				Troop troop;
				troop.pos = { 6, 3 };
				troop.type = Troop::Archer;
				troop.hp = troop.max_hp();
				map.setTroop(troop);

				troop.pos = { 2, 3 };
				troop.type = Troop::Spearman;
				troop.hp = troop.max_hp();
				map.setTroop(troop);

				troop.pos = { 4, 4 };
				troop.type = Troop::Baron;
				troop.hp = troop.max_hp();
				map.setTroop(troop);

				// range test
				{
					troop.pos = { 12, 1 };
					troop.type = Troop::Lumberjack;
					troop.hp = troop.max_hp();
					map.setTroop(troop);

					troop.pos = { 13, 1 };
					troop.type = Troop::Lumberjack;
					troop.hp = troop.max_hp();
					map.setTroop(troop);

					troop.pos = { 14, 1 };
					troop.type = Troop::Farmer;
					troop.hp = troop.max_hp();
					map.setTroop(troop);
				};
				{
					troop.pos = { 10, 3 };
					troop.type = Troop::Archer;
					troop.hp = troop.max_hp();
					map.setTroop(troop);

					troop.pos = { 12, 2 };
					troop.type = Troop::Lumberjack;
					troop.hp = troop.max_hp();
					map.setTroop(troop);

					troop.pos = { 14, 3 };
					troop.type = Troop::Farmer;
					troop.hp = troop.max_hp();

					troop.addEffect(EffectType::Poisoned);
					troop.addEffect(EffectType::RangeBoost);
					troop.addEffect(EffectType::OffenseBoost);
					troop.addEffect(EffectType::DefenseBoost);
					troop.addEffect(EffectType::Stunned);
					troop.addEffect(EffectType::Shielded);
					map.setTroop(troop);

					troop.pos = { 14, 4 };
					map.setTroop(troop);
				};

				// tent test
				{
					Troop troop;
					troop.pos = { 20, 2 };
					troop.type = Troop::Knight;
					troop.hp = 5;
					map.setTroop(troop);

					troop.pos = { 20, 4 };
					troop.type = Troop::Knight;
					troop.hp = 4;
					map.setTroop(troop);

					troop.pos = { 22, 3 };
					troop.type = Troop::Baron;
					troop.hp = 1;
					map.setTroop(troop);
				};
				{
					Build build;
					build.type = Build::Tent;
					build.pos = { 21, 3 };
					map.setBuild(build);
				};

				// plants test
				{
					Troop troop;
					troop.pos = { 20, 0 };
					troop.type = Troop::Farmer;
					troop.hp = troop.max_hp();
					map.setTroop(troop);

					Plant plant;
					plant.pos = { 21, 0 };
					plant.type = Plant::Peach;
					map.setPlant(plant);
				};
				{
					Troop troop;
					troop.pos = { 20, 6 };
					troop.type = Troop::Lumberjack;
					troop.hp = troop.max_hp();
					map.setTroop(troop);

					Plant plant;
					plant.pos = { 21, 6 };
					plant.type = Plant::Tree;
					map.setPlant(plant);
				};
			};

			map.at({ 3, 3 })->region()->money = 111;
			map.at({ 3, 3 })->region()->peach = 11;
			map.at({ 5, 3 })->region()->money = 20;

			map.at({ 9, 3 })->region()->money = 100;
			map.at({ 9, 3 })->region()->peach = 100;

			map.at({ 14, 3 })->region()->money = 100;
			map.at({ 21, 3 })->region()->money = 1000;
			map.at({ 21, 3 })->region()->berry = 12;

			map.at({ 20, 0 })->region()->money = 50;
			map.at({ 20, 6 })->region()->money = 50;
		};
		layer_map->add(game);

		// developer panel
		layer_gui->add(dev::Factory::game_panel(game));
	};

	auto test_ctx = itf.newContext();
	{
		auto layer_test = itf.layer();

		// text field test
		auto* input = new ui::TextField(gameui::Action::textures[0], Values::resource_text, sf::Color::White);
		input->bounds = { 50px, 50px, 600px, 100px };
		layer_test->add(input);
		input->focus(true);

		// key repeat test
		auto* solid = new ui::Solid;
		solid->bounds = { 50px, 200px, 50px, 50px };
		solid->color = sf::Color::Green;
		solid->onEvent([=](const ui::Event& evt) {
			if (auto data = evt.get<ui::Event::KeyPress>()) {
				if (data->key == sf::Keyboard::Key::Q) {
					ui::Anim* anim = new ui::AnimSet(sf::seconds(0.5f), [=](float t) {
						solid->bounds.position.x.px = ui::lerpf(50, 250, t);
					});
					solid->push(anim);
					return true;
				};
			};
			return false;
		});
		layer_test->add(solid);
	};

	itf.lock();
	itf.switchContext(game_ctx);

	// window main loop
	while (ui::window.active()) {
		for (const auto& event : ui::window.events()) {
			if (auto data = event.getIf<sf::Event::KeyPressed>()) {
				if (data->code == sf::Keyboard::Key::F1 && !data->control) {
					itf.switchContext(game_ctx);
				};
				if (data->code == sf::Keyboard::Key::F2 && !data->control) {
					itf.switchContext(test_ctx);
				};
				if (data->code == sf::Keyboard::Key::F3) {
					using namespace Serialize;

					auto temp = Template::generate(&game->map);

					sf::Packet pack;
					pack << temp;

					auto data = reinterpret_cast<const uint8_t*>(pack.getData());

					Template t;
					pack >> t;

					t.construct(&game->map);
				};

				// reload translation file
				if (data->code == sf::Keyboard::Key::Insert && data->control) {
					assets::loadLanguage("en-us.tlml");
				};
			};
		};
		ui::window.frame();
	};
	return 0;
};