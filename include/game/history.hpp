#pragma once

// include dependencies
#include "moves/move.hpp"
#include "moves/troop_moves.hpp"
#include "moves/build_moves.hpp"
#include <vector>
#include <memory>
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

	/// Adds a new move to history.
	/// 
	/// Added move will be immediately applied.
	/// 
	/// @param move Allocated move descriptor.
	void add(Move* move);

	/// Undoes the last move.
	///
	/// Does nothing if no moves are left.
	void undo();

	/// Redoes the last move.
	///
	/// Does nothing if no moves were undone.
	void redo();

	/// Returns current move list.
	std::span<const std::unique_ptr<Move>> list() const;

	/// Returns amount of moves done & reverted. 
	std::pair<size_t, size_t> count() const;
};