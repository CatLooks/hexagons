#pragma once

// include dependencies
#include "adapter.hpp"
#include "game/map.hpp"
#include "game/logic/turn_logic.hpp"

/// Game state controller.
class GameState {
public:
	/// Player description.
	struct Player {
		Region::Team team; /// Player team.
	};

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

	std::vector<Player> _plr {}; /// Player list.
	size_t              _idx {}; /// Current player index.

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
	void addPlayer(const Player& player);

	/// Updates map object reference.
	/// 
	/// @param map Map object reference.
	void setMap(Map* map);

	/// Initializes the game.
	void init();
	/// Attempts to finish a move.
	///
	/// @return Whether the attempt succeeded.
	bool finish();

	/// Advances the game to the next player.
	void next();
	/// Processes incoming messages.
	void tick();
	/// Processes a single event.
	///
	/// @param event Event data.
	void proc(const Adapter::Packet<Adapter::Event>& event);

	/// Returns current player info.
	const Player* player() const;

	/// Returns local player team.
	Region::Team team() const;
};