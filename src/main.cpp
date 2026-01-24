#include "ui.hpp"
#include "game.hpp"
#include "assets.hpp"
#include "flags.hpp"
#include "menu.hpp"
#include "networking/Net.hpp"
#include "game/sync/network_adapter.hpp" 
#include "game/loader.hpp"
#include "logging/log.hpp"

#include "game/serialize/map.hpp"
#include <exception>

int main() {
	try {
		logging::redirect_stdout_stderr();

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

		auto* adapter = new BotAdapter(0.f);
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

		auto* edit_adapter = new BotAdapter(0.f);
		GameState edit_state(GameState::Edit, edit_adapter);

		auto edit_ctx = itf.newContext();
		Game* edit_game = nullptr;
		{
			auto layer_map = itf.layer();
			auto layer_gui = itf.layer();
			auto layer_msg = itf.layer();

			edit_game = new Game(layer_map, layer_gui, layer_msg, &edit_state);
			edit_game->getEdit()->refreshAllText();
			layer_map->add(edit_game);
			layer_gui->add(dev::Factory::game_panel(edit_game));
		};

		MenuSystem menuSystem(itf, &game_ctx, &edit_ctx, game, net, state, edit_game);
		itf.switchContext(menuSystem.context);

		edit_game->add(new ui::Exit([&]() {
			itf.switchContext(menuSystem.context);
		}, 0px));

		while (ui::window.active()) {
			net.fetch();
			ui::window.events();
			ui::window.frame();
		}

		return 0;
	}
	catch (const std::exception& exc) {
		fprintf(stderr, "[FATAL] Unhandled exception: %s\n", exc.what());
		return 2;
	}
	catch (...) {
		fprintf(stderr, "[FATAL] Unhandled unknown exception.\n");
		return 3;
	}
}
