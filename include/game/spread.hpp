#pragma once

// include dependencies
#include <functional>
#include <deque>
#include "array.hpp"

/// Effect spread description.
///
/// Conditionally applies a function to nearby tiles.
struct Spread {
	/// Spread target information.
	struct Tile : HexRef {
		size_t left {}; /// Amount of hops left.
	};

	/// Condition for a spreader function to affect the tile.
	///
	/// @param tile Spread target information.
	using Check = std::function<bool(const Tile& tile)>;
	/// Applies an effect on a tile affected by spreader.
	///
	/// @param tile Spread target information.
	using Effect = std::function<void(Tile& tile)>;

	/// Default spread check.
	/// 
	/// @return `true` for any hex.
	static bool default_check(const Tile&);
	/// Default spread effect.
	///
	/// Performs no action.
	static void default_effect(Tile&);

	/// Blocking check.
	///
	/// If this check fails, the spread will not propagate to neighboring tiles.
	///
	/// By default, spread is not limited.
	Check hop     = default_check;
	/// Non-blocking check.
	///
	/// This check is only used to conditionally apply the effect.
	///
	/// By default, spread is not limited.
	Check pass    = default_check;
	/// Tile effect function.
	///
	/// Gets applied to every reached hex if `pass()` check passes.
	/// 
	/// By default, does nothing.
	Effect effect = default_effect;
	/// Whether the spread affects the origin tile.
	bool imm      = false;

	/// Generates a unique spread pass index.
	/// 
	/// Spread index is used to mark tiles as "visited".
	/// Any tile whose spread index does not match is treated as "not visited".
	/// 
	/// @return New unique non-zero integer.
	static size_t index();

	/// Applies the spread.
	/// 
	/// The tile located at origin does *not* get affected, but is marked as "visited".
	/// 
	/// @param array Target tile array.
	/// @param pos Spread origin.
	/// @param radius Maximum spread radius (infinite by default).
	///
	/// @return Spread index.
	size_t apply(const HexArray& array, sf::Vector2i pos, size_t radius = ~0ull) const;

	/// Visited tile coordinate list.
	///
	/// Returned by `spreadlist()`.
	using List = std::deque<sf::Vector2i>;

	/// Applies the spread.
	/// 
	/// The tile located at origin does *not* get affected, but is marked as "visited".
	/// 
	/// @param array Target tile array.
	/// @param pos Spread origin.
	/// @param radius Maximum spread radius (infinite by default).
	/// 
	/// @return List of coordinates of all visited tiles.
	List applylist(const HexArray& array, sf::Vector2i pos, size_t radius = ~0ull) const;
};