#include <SFML/Graphics.hpp>
#include "ui/solid.hpp"
#include "ui/layer.hpp"
#include "ui/panel.hpp"
#include "ui/text.hpp"
#include "ui/image.hpp"
#include "ui/anim/linear.hpp"
#include "assets.hpp"
#include "game.hpp"
#include "eos/Config.hpp"
#include "eos/HWID.hpp"
#include "eos/PlatformManager.hpp"
#include "eos/LoggingManager.hpp"
#include "eos/LobbyManager.hpp"
#include "eos/P2PManager.hpp"

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
	if (assets::error) return 1;

	// initialize EOS
	PlatformManager* platformManager = new PlatformManager();
	EOSSdkConfig* sdkConfig = new EOSSdkConfig();
	platformManager->InitializeEOSSdk(sdkConfig);
	platformManager->CreatePlatformInstance(sdkConfig);
	delete sdkConfig;

	LoggingManager* loggingManager = new LoggingManager();
	loggingManager->RegisterLoggingCallbackExample();
	loggingManager->SetLogLevelVeryVerboseExample();

	HWID* hwid = new HWID();
	hwid->Login(platformManager->GetPlatformHandle());


	// create window interface
	ui::Interface itf;
	ui::Layer* map_layer = itf.layer(&assets::tiles);

	// create game element
	GameMap* game = new GameMap(&itf);
	map_layer->add(game);

	// setup window
	sf::RenderWindow win(sf::VideoMode({ 1600, 900 }), "App");
	win.setVerticalSyncEnabled(true);
	win.setKeyRepeatEnabled(false);

	// window main loop
	while (win.isOpen()) {
		win.clear(sf::Color(34, 39, 41));

		// update external state
		game->mouse = sf::Mouse::getPosition(win);
		game->focused = win.hasFocus();

		// update interface
		itf.recalculate(win.getSize());
		while (const auto event = win.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				win.close();
			};
			itf.event(*event);
		};
		itf.update(sf::Mouse::getPosition(win));

		// draw interface
		itf.draw(win);
		win.display();
	};
	return 0;
};