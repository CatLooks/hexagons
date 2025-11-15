#include "game/map.hpp"
#include "game/draw.hpp"

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

/// Shifts map camera.
void Map::shiftCamera(sf::Vector2i offset) {
	_camera += offset;
};

/// Checks if a position is within the map.
bool Map::contains(sf::Vector2i pos) const {
	return (
		// vertical check
		(pos.y >= 0 && pos.y < _size.y) &&
		// horizontal check (ignore last if shifted)
		(pos.x >= 0 && pos.x < _size.x - (pos.y & 1 ? 0 : 1))
	);
};

/// Returns a reference to a tile at position.
Hex* Map::at(sf::Vector2i pos) const {
	return contains(pos)
		? &_tiles.get()[pos.y * _size.x + pos.x]
		: nullptr;
};
/// Returns a reference to a tile at position.
Hex* Map::operator[](sf::Vector2i pos) const {
	return at(pos);
};

/// Returns amount of hexes stored.
size_t Map::tilecount() const {
	return (size_t)_size.x * _size.y;
};

/// Generates a copy of the map.
Map Map::copy() const {
	// copy tilemap
	size_t count = tilecount();
	Hex* tilemap = new Hex[count];
	for (size_t i = 0; i < count; i++)
		tilemap[i] = _tiles.get()[i];

	// create new map
	Map map;
	map._tiles = std::unique_ptr<Hex[]>(tilemap);
	map._size = _size;
	map._troops = _troops;
	map._builds = _builds;
	map._plants = _plants;
	return map;
};

/// Resizes the map.
void Map::resize(sf::IntRect rect) {
	// create new tilemap
	size_t count = (size_t)rect.size.x * rect.size.y;
	Hex* tilemap = new Hex[count];

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
				tilemap[new_pos.y * rect.size.x + new_pos.x]
					= _tiles.get()[old_pos.y * _size.x + old_pos.x];
			};
		};
	};

	// swap tilemap
	_tiles = std::unique_ptr<Hex[]>(tilemap);
	_size = rect.size;
};

/// Returns backplane rectangle.
sf::IntRect Map::backplane() const {
	// calculate height
	int y = Values::tileUnit;
	if (_size.y > 1)
		y += Values::tileOff.y * (_size.y - 1);

	// return backplane rectangle
	return {
		-Values::mapBorder,
		sf::Vector2i(Values::tileOff.x * _size.x, y) + Values::mapBorder * 2
	};
};

/// Draws the map.
void Map::draw(ui::RenderBuffer& target) const {
	// draw backplane
	sf::IntRect bp = backplane();
	target.quad(
		{ bp.position - _camera, bp.size },
		{}, sf::Color(40, 42, 48)
	);
	target.forward(nullptr);

	// calculate drawn area
	// @todo
	sf::IntRect area = { {}, _size };
	sf::Vector2i origin = -_camera;

	// draw tile geometry
	TileDrawer drawer(*this, area, origin);
	while (auto tile = drawer.next()) {
		tile->drawBase(target);
		tile->drawSides(target, sf::Color::Black);
	};

	// draw borders
	drawer.reset();
	while (auto tile = drawer.next()) {
		tile->drawBorders(target, Draw::regionBorderTest, sf::Color::Black);
	};
};