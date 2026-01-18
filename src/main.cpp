#include "ui.hpp"
#include "game.hpp"
#include "assets.hpp"
#include "flags.hpp"
#include "menu.hpp"
#include "networking/Net.hpp"


int main() {
	// This automatically calls loadLanguageList() and loadLanguage() for the default entry.
	assets::lang::init();

	assets::loadAssets();
	if (assets::error) {
		fprintf(stderr, "Critical error: Failed to load game assets.\n");
		return 1;
	}

	// initialize networking
	EOSManager* eos = &EOSManager::GetInstance();
	Net net;

	// create window
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

	// Game State and Context Setup
	GameState state(GameState::Host, new TestAdapter);
	auto game_ctx = itf.newContext();
	Game* game_instance = nullptr; 
	{
		itf.switchContext(game_ctx);

		auto layer_map = itf.layer();
		auto layer_gui = itf.layer();
		auto layer_msg = itf.layer();

		game_instance = new Game(layer_map, layer_gui, layer_msg, &state);
		
		state.addPlayer({ .name = "Player", .team = Region::Red });
		state.addPlayer({ .name = "Bot 1", .team = Region::Orange });
		state.init();

		game_instance->map.empty({ 24, 7 });
		game_instance->map.regions.enumerate(&game_instance->map);

		layer_map->add(game_instance);
		layer_gui->add(dev::Factory::game_panel(game_instance));
	}

	MenuSystem menuSystem(itf, &game_ctx, game_instance);
	itf.switchContext(menuSystem.context);

	while (ui::window.active()) {
		net.fetch();
		ui::window.events();
		ui::window.frame();
	}

	return 0;
}