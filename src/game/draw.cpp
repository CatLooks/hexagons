#include "game/draw.hpp"

namespace Draw {
	/// Constructs tile drawing data.
	Tile::Tile(const Map& map, sf::Vector2i coords, sf::Vector2i origin)
		: map(map), hex(map.at(coords)), coords(coords), origin(origin) {};

	/// Draws tile base.
	void Tile::drawBase(ui::RenderBuffer& target) const {
		// draw ground tiles
		if (hex->type == Hex::Ground) {
			target.quad(
				{ origin, {TILE, TILE}},
				Textures::hex(hex->team)
			);
			target.forward(&assets::tilemap);
		};

		// draw water tiles
		if (hex->type == Hex::Water) {
			target.quad({ origin + sf::Vector2i(0, TILE_WATER), {TILE, TILE}}, Textures::hexWater);
			target.forward(&assets::tilemap);
		};
	};

	/// Draws tile borders.
	void Tile::drawBorders(ui::RenderBuffer& target, sf::Color color) const {
		// ignore if not a ground tile
		if (hex->type != Hex::Ground) return;

		// border index
		// each bit corresponds to a border
		uint8_t border = 0;

		// check neighbors
		for (int i = 0; i < 6; i++) {
			// fetch neighbor hex
			const Hex* nb = map[map.neighbor(coords, static_cast<Map::nbi_t>(i))];

			// set border if passes check
			if (!nb || nb->type != Hex::Ground || nb->team != hex->team)
				border |= 1 << i;
		};

		// ignore if no borders
		if (!border) return;

		// convert border index into border texture coords
		sf::IntRect texmap = { { (border & 7) * 64, (border >> 3) * 64 }, { 64, 64 } };

		// draw borders
		target.quad({ origin, { TILE, TILE } }, texmap, color);
		target.forward(&assets::borders);
	};
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
	_draw = _org;

	// add shifted row offset
	if (!(_coords.y & 1))
		_draw.x += TILE_ROW_OFF;
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
			_draw.x = _org.x + (_coords.y & 1 ? 0 : TILE_ROW_OFF);
			_draw.y += TILE_Y_OFF;
		}
		else {
			_draw.x += TILE_X_OFF;
		};

		// return if current tile is valid
		if (tile->hex) return tile;
	};
};