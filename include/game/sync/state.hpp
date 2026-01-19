#pragma once

// include dependencies
#include "adapter.hpp"
#include "game/map.hpp"
#include "game/logic/turn_logic.hpp"
#include "game/ui/chat.hpp"
#include "game/ui/splash_text.hpp"
#include "dev/dev_game.hpp"
#include <SFML/System/Clock.hpp>

/// Game state controller.
class GameState {
	friend dev::Factory;

public:
	using Player = Messages::Player;

	/// Game controller mode.
	enum Mode {
		Host,  /// Player acts as a host.
		Client /// Player acts as a client.
	};

	/// Current game state.
	enum State {
		Quit, /// Bad game state.
		Init, /// State initialization.
		Wait, /// Player is waiting for a move.
		Play, /// Player is making a move.
	};

private:
	/// Communication adapter.
	std::unique_ptr<Adapter> _adapter;

	Map*    _map; /// Map reference.
	State _state; /// Current game state.
	Mode   _mode; /// Game controller mode.

	/// Chat element reference.
	gameui::Chat* _chat;
	/// Splash element reference.
	gameui::Splash* _splash;

	std::vector<Player> _plr;     /// Player list.
	uint32_t            _idx = 0; /// Current player index.
	uint32_t           _turn = 1; /// Current turn number.
	sf::Clock         _clock;     /// Current turn time.

	/// Player update callback.
	std::function<void(bool enable)> _call;

public:
	/// Constructs a game state object.
	/// 
	/// @param mode Game controller mode.
	/// @param adapter Communication adapter.
	GameState(Mode mode, Adapter* adapter);

	/// Adds a player update callback.
	/// 
	/// @param enabled Whether the local player is currently making a move.
	void updateCallback(std::function<void(bool enabled)> callback);

	/// Adds a new player to the game.
	/// 
	/// @param player Player info.
	void addPlayer(const Messages::Player& player);

	/// Updates object references.
	/// 
	/// @param map Map object reference.
	/// @param chat Chat element reference.
	/// @param splash Splash element reference.
	void setRefs(Map* map, gameui::Chat* chat, gameui::Splash* splash);

protected:
	/// Updates gameplay state.
	void update();
	/// Locks gameplay state.
	void lock();

public:
	/// Sends a message to chat.
	/// 
	/// @param text Message text.
	void message(const std::string& text);

	/// Initializes the game.
	void init();
	/// Attempts to finish a move.
	///
	/// @return Whether the attempt succeeded.
	bool finish();

	/// Finishes the game.
	/// 
	/// @param id Victor index.
	void over(size_t id);

	/// Advances the game to the next player.
	void next();
	/// Processes incoming messages.
	void tick();
	/// Processes a single event.
	///
	/// @param event Event data.
	void proc(const Adapter::Packet<Messages::Event>& event);

	/// Returns current player info.
	const Player* player() const;
	/// Returns next player info.
	const Player* nextPlayer() const;
	/// Returns current turn time.
	sf::Time turnTime() const;

	/// Returns local player team.
	Region::Team team() const;

	/// Returns current turn number.
	uint32_t turn() const;
};