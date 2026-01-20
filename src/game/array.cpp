#include "game/array.hpp"

/// Returns neighbor position of a tile.
sf::Vector2i HexArray::neighbor(sf::Vector2i pos, nbi_t nbi) {
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

/// Constructs an empty array.
HexArray::HexArray() {};
/// Destroys the array.
HexArray::~HexArray() { clear(); };

/// Move constructor.
HexArray::HexArray(HexArray&& array) noexcept
	: _tiles(array._tiles), _size(array._size)
{
	array._tiles = nullptr;
};
/// Move assignment.
HexArray& HexArray::operator=(HexArray&& array) noexcept {
	this->~HexArray();
	{
		_tiles = array._tiles;
		_size = array._size;
		array._tiles = nullptr;
	};
	return *this;
};

/// Returns array size.
sf::Vector2i HexArray::size() const {
	return _size;
};
/// Returns hex count.
size_t HexArray::count() const {
	return (size_t)_size.x * _size.y;
};

/// Checks if a position is within the map.
bool HexArray::contains(sf::Vector2i pos) const {
	return (
		// vertical check
		(pos.y >= 0 && pos.y < _size.y) &&
		// horizontal check (ignore last if shifted)
		(pos.x >= 0 && pos.x < _size.x - (pos.y & 1 ? 0 : 1))
	);
};

/// Unsafe tile look-up.
Hex& HexArray::ats(sf::Vector2i pos) const {
	return _tiles[pos.y * _size.x + pos.x];
};

/// Returns a reference to a tile at position.
Hex* HexArray::at(sf::Vector2i pos) const {
	return contains(pos) ? &ats(pos) : nullptr;
};

/// Returns a reference to a tile at position.
Hex* HexArray::operator[](sf::Vector2i pos) const {
	return at(pos);
};

/// Returns a complete reference to a tile.
HexRef HexArray::atref(sf::Vector2i pos) const {
	return HexRef(at(pos), pos);
};

/// Clears the array.
void HexArray::clear() {
	delete[] _tiles;
	_tiles = nullptr;
	_size = {};
};

/// Empties the array and sets the new size.
void HexArray::empty(sf::Vector2i size) {
	clear();
	_size = size;
	_tiles = new Hex[count()];
};

/// Resizes the array.
void HexArray::resize(sf::IntRect rect) {
	HexArray array;
	array.empty(rect.size);

	// copy old tiles
	sf::IntRect prev = { {}, _size };
	auto is = prev.findIntersection(rect);
	if (is) {
		for (int dy = 0; dy < is->size.y; dy++) {
			for (int dx = 0; dx < is->size.x; dx++) {
				// old tile position
				sf::Vector2i old_pos = sf::Vector2i(dx, dy) + is->position;
				// new tile position
				sf::Vector2i new_pos = old_pos - rect.position;

				// copy the tile
				array.ats(new_pos) = std::move(ats(old_pos));
			};
		};
	};
	*this = std::move(array);
};