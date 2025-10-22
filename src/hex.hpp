#pragma once

// hex tile object
struct Hex {
	// hex type
	enum Type: uint8_t {
		Void,
		Land
	} type = Void;

	// team capture
	enum Team: uint8_t {
		None,
		Red,
		Blue
	} team = None;

	// spread buffer
	size_t buffer = 0ull;

	// troop on the hex (pool index)
	// -1 if no troop
	size_t troop = ~0ull;
};

// return hex neighbor coordinate
// @param idx Neighbor index.
//   5   0
//
// 4   #   1
//
//   3   2
sf::Vector2i hexNeighbor(sf::Vector2i pos, size_t idx) {
	// look-up tables
	static const int dy[6] { -1, 0, 1, 1, 0, -1 };
	static const int dx[2][6] {
		{ 1, 1, 1, 0, -1, 0 },   // y & 1 = 0
		{ 0, 1, 0, -1, -1, -1 }, // y & 1 = 1
	};

	// return neighbor position
	return pos + sf::Vector2i(dx[pos.y & 1][idx], dy[idx]);
};