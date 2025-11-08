#include "game/map.hpp"
#include "div.hpp"

/// Returns backplane rectangle.
sf::IntRect Map::backplane() const {
	// calculate height
	int y = 0;
	if (_size.x == 1) y = TILE;
	else if (_size.x > 1) y = TILE_X_OFF * _size.x - (TILE - TILE_Y_OFF);

	// return rectangle
	return {
		sf::Vector2i(-MAP_BORDER, -MAP_BORDER),
		sf::Vector2i(TILE * _size.x, y)
			+ sf::Vector2i(MAP_BORDER * 2, MAP_BORDER * 2)
	};
};

/// Draws the map.
void Map::draw(ui::RenderBuffer& target) const {
	// get first top-left visible tile
	sf::Vector2i origin = {};

	// draw backplane
	sf::IntRect bp = backplane();
	target.quad(bp, {}, sf::Color(64, 66, 72));
	target.forward(nullptr);

	// draw every hex
	for (int dy = 0; dy < _size.y; dy++) {
		// row y
		int y = dy;

		// get row position
		sf::Vector2i row = {
			(y & 1 ? 0 : TILE_ROW_OFF),
			y * (TILE_Y_OFF)
		};

		// draw rows
		for (int dx = 0; dx < _size.x; dx++) {
			// row x
			int x = dx;

			// get tile position
			sf::Vector2i pos = row + sf::Vector2i(x * TILE_X_OFF, 0);
		};
	};
};