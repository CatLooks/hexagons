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
	redo();
};

/// Undoes the last move.
std::optional<sf::Vector2i> History::undo() {
	// ignore if first move
	if (_cursor == 0) return {};

	// undo current move
	Move* move = _list[--_cursor].get();
	move->revert(_map);
	return move->revertCursor();
};

/// Redoes the last move.
std::optional<sf::Vector2i> History::redo() {
	// ignore if last move
	if (_cursor >= _list.size()) return {};

	// redo current move
	Move* move = _list[_cursor++].get();
	move->apply(_map);
	return move->applyCursor();
};

/// Returns a list of applied moves.
History::SpanList History::list() const {
	return SpanList(_list.begin(), _list.begin() + _cursor);
};

/// Returns amount of moves done & reverted. 
std::pair<size_t, size_t> History::count() const {
	return std::make_pair(_cursor, _list.size() - _cursor);
};

/// Returns last move.
const Move* History::last() const {
	// ignore if no moves
	if (_list.empty() || _cursor == 0) return nullptr;

	// return last move
	return _list[_cursor - 1].get();
};

/// Returns next move.
const Move* History::next() const {
	// ignore if no moves
	if (_cursor >= _list.size()) return nullptr;

	// return next move
	return _list[_cursor].get();
};