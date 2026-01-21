#include "ui.hpp"
#include "game.hpp"
#include "assets.hpp"
#include "flags.hpp"
#include "menu.hpp"
#include "networking/Net.hpp"
#include "game/sync/network_adapter.hpp" 
#include "game/loader.hpp"



#include "game/serialize/map.hpp"
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

	auto* adapter = new BotAdapter(1.f);
	GameState state(GameState::Host, adapter);

	auto game_ctx = itf.newContext();
	Game* game = nullptr;
	{
		itf.switchContext(game_ctx);

		auto layer_map = itf.layer();
		auto layer_gui = itf.layer();
		auto layer_msg = itf.layer();

		game = new Game(layer_map, layer_gui, layer_msg, &state);
		adapter->map = &game->map;

		layer_map->add(game);
		layer_gui->add(dev::Factory::game_panel(game));
	}

	MenuSystem menuSystem(itf, &game_ctx, game, net, state);	
	itf.switchContext(menuSystem.context);


	while (ui::window.active()) {
		net.fetch();
		ui::window.events();
		ui::window.frame();
	}

	return 0;
}