#include "game/draw.hpp"

namespace Draw {
	/// Constructs tile drawing data.
	Tile::Tile(const Map* map, sf::Vector2i coords, sf::Vector2i origin, sf::Vector2i size)
		: map(map), hex(map->at(coords)), coords(coords), origin(origin), size(size) {};
};

/// Constructs a tile drawer.
TileDrawer::TileDrawer(const Map* map, sf::IntRect area, sf::Vector2i origin, sf::Vector2i size):
	_map(map),
	_size(size),
	_org(origin),
	_beg(area.position),
	_end(area.position + area.size)
{
	reset();
};

/// Selects a tile for elevation.
void TileDrawer::select(sf::Vector2i coords, int elevation) {
	_select = coords;
	_elev = elevation;
};

/// @return Selected tile coordinates.
sf::Vector2i TileDrawer::selected() const {
	return _select;
};

/// Checks whether the tile is selected.
bool TileDrawer::selected(const Draw::Tile& tile) const {
	return _select == tile.coords;
};

/// Resets the drawer to first tile.
void TileDrawer::reset() {
	_coords = _beg;
	_draw = _org + Values::rowOffset(_coords.y);
};

/// Returns the next tile drawing data.
std::optional<Draw::Tile> TileDrawer::next() {
	while (1) {
		// check for overflow
		if (_coords.y >= _end.y)
			return {};

		// store current tile
		std::optional<Draw::Tile> tile = Draw::Tile(
			_map, _coords,
			_draw + sf::Vector2i(0, (_coords == _select) ? -_elev : 0),
			_size
		);

		// calculate next tile coordinates
		if (++_coords.x >= _end.x - (_coords.y & 1 ? 0 : 1)) {
			// draw next row
			_coords.x = _beg.x;
			_coords.y++;

			// advance draw origin to next row
			_draw.x = _org.x + Values::rowOffset(_coords.y).x;
			_draw.y += Values::tileOff.y;
		}
		else {
			_draw.x += Values::tileOff.x;
		};

		// return if current tile is valid
		if (tile->hex) return tile;
	};
};