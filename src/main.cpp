#include <SFML/Graphics.hpp>
#include "ui/solid.hpp"
#include "ui/layer.hpp"
#include "ui/panel.hpp"
#include "ui/text.hpp"
#include "ui/image.hpp"
#include "ui/anim/linear.hpp"
#include "assets.hpp"
#include "game.hpp"
#include "networking_remake/Net.hpp"


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
	EOSManager* eos = &EOSManager::GetInstance();
	// Use Net as the external networking API
	Net net;

	// connection logic temporary input
	int temp;
	std::cout << "Host: 1, Client: 2" << std::endl;
	std::cin >> temp;

	if (temp == 1) {
		net.host();
	}
	else {
		net.connect();
	}

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

		// drive EOS/net processing
		net.fetch();

		// handle incoming network events
		while (auto ev = net.next()) {
			if (std::holds_alternative<NetConnected>(*ev)) {
				auto c = std::get<NetConnected>(*ev);
				std::cout << "[Net] Connected: " << c.userId << std::endl;
			}
			else if (std::holds_alternative<NetDisconnected>(*ev)) {
				auto d = std::get<NetDisconnected>(*ev);
				std::cout << "[Net] Disconnected: " << d.userId << std::endl;
			}
			else if (std::holds_alternative<NetPacket>(*ev)) {
				auto p = std::get<NetPacket>(*ev);
				// interpret payload as string for simple demo
				std::string msg(p.data.begin(), p.data.end());
				std::cout << "[Net] Packet from " << p.senderId << ": " << msg << std::endl;
			}
		}

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