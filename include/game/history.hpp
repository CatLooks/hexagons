#pragma once

// include dependencies
#include "moves.hpp"
#include <memory>
#include <optional>
#include <span>

/// History of reversible moves on a game map.
class History {
private:
	/// Move list.
	std::vector<std::unique_ptr<Move>> _list;
	/// Currently shown move.
	size_t _cursor {};
	/// Map reference.
	Map* _map {};

public:
	/// Constructs move history for a game map.
	/// 
	/// @param map Map reference.
	History(Map* map);

	/// Clears the history.
	void clear();

	/// Adds a new move queue to history.
	/// 
	/// Added move will be immediately applied.
	/// 
	/// @param move Allocated move object.
	void add(Move* move);

	/// Undoes the last move.
	///
	/// Does nothing if no moves are left.
	/// 
	/// @return New cursor position.
	std::optional<sf::Vector2i> undo();
	/// Redoes the last move.
	///
	/// Does nothing if no moves were undone.
	/// 
	/// @return New cursor position.
	std::optional<sf::Vector2i> redo();

	/// Reference move list type.
	using RList = std::span<const std::unique_ptr<Move>>;
	/// Transfer move list type.
	using TList = std::vector<std::unique_ptr<Move>>;

	/// Returns a list of applied moves.
	RList list() const;
	/// Returns amount of moves done & reverted. 
	std::pair<size_t, size_t> count() const;

	/// Returns last move.
	const Move* last() const;
	/// Returns next move.
	const Move* next() const;
};