#include "game/ui/hex_preview.hpp"

namespace gameui::preview {
	// 1x1 map
	Map map = []() {
		Map map;
		map.resize({ {}, {2, 1} });
		map.at({})->type = Hex::Ground;
		return map;
	}();
	Draw::Tile tile = Draw::Tile(&map, {}, {}, Values::tileSize / 2);

	/// Draws the preview tile.
	void draw(ui::RenderBuffer& target, sf::IntRect self) {
		// set tile position
		tile.hex = map.at({});
		tile.origin = self.position + (self.size - Values::tileSize / 2) / 2;

		/* draw base    */ tile.drawBase(target);
		/* draw sides   */ tile.drawSides(target, sf::Color::Black, sf::Color::Black);
		/* draw borders */ tile.drawBorders(target, sf::Color::Black);
	};
};