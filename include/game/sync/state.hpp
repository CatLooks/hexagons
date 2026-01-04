#pragma once

// include dependencies
#include "adapter.hpp"
#include "game/map.hpp"

/// Game state controller.
class GameState {
public:
	/// Current game state.
	enum Mode {
		Init, /// State initialization.
		Wait, /// Player is waiting for a move.
		Play, /// Player is making a move.
	};

private:
	/// Communication adapter.
	std::unique_ptr<Adapter> _adapter;

	Map&  _map; /// Map reference.
	Mode _mode; /// Current game state.

public:
	Region::Team team; /// Player team.

public:
	/// Constructs a game state object.
	/// 
	/// @param map Map reference.
	/// @param adapter Communication adapter.
	GameState(Map& map, Adapter* adapter);

	/// Attempts to finish a move.
	///
	/// @return Whether the attempt succeeded.
	bool finish();
};