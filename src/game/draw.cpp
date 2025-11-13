#include "game/draw.hpp"

namespace Draw {
	/// Constructs tile drawing data.
	Tile::Tile(const Map& map, sf::Vector2i coords, sf::Vector2i origin)
		: map(map), hex(map.at(coords)), coords(coords), origin(origin) {};
};

/// Constructs a tile drawer.
TileDrawer::TileDrawer(const Map& map, sf::IntRect area, sf::Vector2i origin):
	_map(map),
	_org(origin),
	_beg(area.position),
	_end(area.position + area.size)
{
	reset();
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
		std::optional<Draw::Tile> tile = Draw::Tile(_map, _coords, _draw);

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