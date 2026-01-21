#include "game/spread.hpp"
#include <list>

/// Default spread check.
bool Spread::default_check(const Tile&) { return true; };
/// Default spread effect.
void Spread::default_effect(const Tile&) {};
/// Default radius override.
std::optional<size_t> Spread::default_radius(const Tile&) { return {}; };

/// Default last spread index.
size_t Spread::_last_idx[2] = { 0, 0 };

/// Resets spread index.
void Spread::reset() {
	_last_idx[0] = 0;
	_last_idx[1] = 0;
};

/// Generates a unique spread index.
size_t Spread::index(bool alt) {
	return ++_last_idx[alt];
};

/// Adds neighboring tiles to spread queue.
/// 
/// @param queue Spread queue.
/// @param hop Blocking spread check.
/// @param array Tile array.
/// @param index Spread index.
/// @param tile Processed tile.
/// @param alt Whether to use alternative index.
static inline void _spread(
	std::list<Spread::Tile>& queue,
	const HexArray& array,
	const Spread::Check& hop,
	size_t index,
	const Spread::Tile& tile,
	bool alt
) {
	// ignore if no range left
	if (tile.left == 0) return;

	// spread to each neighbor
	for (int i = 0; i < 6; i++) {
		// next tile info
		Spread::Tile next = {
			array.atref(
				array.neighbor(tile.pos, static_cast<HexArray::nbi_t>(i))
			),
			tile.left - 1
		};

		// discard if outside the map
		if (!next.hex) continue;

		// mark as visited
		if (next.hex->spread[alt] != index)
			next.hex->spread[alt] = index;
		else continue;

		// queue tile if passes blocking check
		if (hop(next))
			queue.push_back(next);
	};
};

/// Applies the spread.
size_t Spread::apply(const HexArray& array, sf::Vector2i pos, size_t radius) const {
	// get new spread index
	size_t idx = index(alt);

	// get origin tile
	Hex* origin = array.at(pos);
	if (!origin) return idx;

	// override radius
	Tile tile = { { origin, pos }, radius };
	if (auto nr = this->radius(tile))
		tile.left = radius = *nr;

	// apply effect to origin if needed
	origin->spread[alt] = idx;
	if (imm && pass(tile))
		effect(tile);
	if (!radius) return idx;

	// tile queue
	std::list<Tile> queue;

	// initial spread from origin
	_spread(queue, array, hop, idx, tile, alt);

	// spreader loop
	while (!queue.empty()) {
		// pull next tile
		Tile tile = queue.front();
		queue.pop_front();

		// apply effect
		if (pass(tile)) effect(tile);

		// spread to neighboring tiles
		_spread(queue, array, hop, idx, tile, alt);
	};

	// return spread index
	return idx;
};

/// Applies the spread.
Spread::List Spread::applylist(const HexArray& array, sf::Vector2i pos, size_t radius) const {
	// affected tile coords list
	List affected;

	// get new spread index
	size_t idx = index(alt);

	// get origin tile
	Hex* origin = array.at(pos);
	if (!origin) return affected;

	// override radius
	Tile tile = { { origin, pos }, radius };
	if (auto nr = this->radius(tile))
		tile.left = radius = *nr;

	// apply effect to origin if needed
	origin->spread[alt] = idx;
	if (imm && pass(tile)) {
		effect(tile);
		affected.push_back(tile.pos);
	};
	if (!radius) return affected;

	// tile queue
	std::list<Tile> queue;

	// initial spread from origin
	_spread(queue, array, hop, idx, { { origin, pos }, radius }, alt);

	// spreader loop
	while (!queue.empty()) {
		// pull next tile
		Tile tile = queue.front();
		queue.pop_front();

		// apply effect
		if (pass(tile)) {
			effect(tile);
			affected.push_back(tile.pos);
		};

		// spread to neighboring tiles
		_spread(queue, array, hop, idx, tile, alt);
	};
	return affected;
};
