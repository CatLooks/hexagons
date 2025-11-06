#include "game/map.hpp"

/// Returns neighbor position of a tile.
sf::Vector2i Map::neighbor(sf::Vector2i pos, nbi_t nbi) {
	// look-up tables
	// horizontal shift
	static const int dx[2][6] = {
		{ 1, 1, 1,  0, -1,  0 }, // shifted row
		{ 0, 1, 0, -1, -1, -1 }, // unshifted row
	};
	// vertical shift
	static const int dy[6] = {
		-1, 0, 1, 1, 0, -1
	};

	// return result using look-up tables
	return pos + sf::Vector2i(dx[pos.y & 1][nbi], dy[nbi]);
};

/// Checks if a position is within the map.
bool Map::contains(sf::Vector2i pos) const {
	return (
		// vertical check
		(pos.y < 0 || pos.y >= _size.y) ||
		// horizontal check (ignore last if shifted)
		(pos.x < 0 || pos.x >= _size.x - (pos.y & 1 ? 1 : 0))
	);
};

/// Returns a reference to a tile at position.
Hex& Map::operator[](sf::Vector2i pos) const {
	return contains(pos)
		? _tiles[pos.y * _size.x + pos.x]
		: _fuse;
};

/// Returns amount of hexes stored.
size_t Map::tilecount() const {
	return (size_t)_size.x * _size.y;
};

/// Generates a copy of the map.
Map Map::copy() const {
	// create new map
	Map map = {
		std::unique_ptr<Hex>(new Hex[tilecount()]),
		_size, _troops, _builds, _plants
	};

	// copy tiles
	for (size_t i = 0; i < tilecount(); i++)
		map._tiles[i] = _tiles[i];
	return map;
};