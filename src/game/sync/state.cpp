#include "game/sync/state.hpp"

/// Constructs a game state object.
GameState::GameState(Map& map, Adapter* adapter)
	: _adapter(adapter), _map(map), _mode(Init)
{

};

/// Attempts to finish a move.
bool GameState::finish() {
	// ignore if not making a move
	if (_mode != Play) return false;

	// transmit move list
	_adapter->send(_map.history.list());

	// reset history
	_map.history.clear();
};