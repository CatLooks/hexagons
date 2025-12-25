#include "game/history.hpp"

/// Constructs move history for a game map.
History::History(Map* map) : _map(map) {};

/// Clears the history.
void History::clear() {
	_list.clear();
	_cursor = {};
};

/// Adds a new move to history.
void History::add(Move* move) {
	// erase reverted moves
	if (_cursor < _list.size())
		_list.erase(_list.begin() + _cursor, _list.end());

	// store and apply new move
	_list.push_back(std::unique_ptr<Move>(move));
	_cursor++;
	move->apply(_map);
};

/// Undoes the last move.
void History::undo() {
	// ignore if first move
	if (_cursor == 0) return;

	// undo current move
	_list[--_cursor].get()->revert(_map);
};

/// Redoes the last move.
void History::redo() {
	// ignore if last move
	if (_cursor >= _list.size()) return;

	// redo current move
	_list[_cursor++].get()->apply(_map);
};

/// Returns amount of moves done & reverted. 
std::pair<size_t, size_t> History::count() const {
	return std::make_pair(_cursor, _list.size() - _cursor);
};