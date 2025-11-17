#include "game/draw/hex_draw.hpp"

namespace Draw {
	/// Draws tile base.
	void Tile::drawBase(ui::RenderBuffer& target) const {
		// draw ground tiles
		if (hex->type == Hex::Ground) {
			target.quad(
				{ origin, Values::tileSize },
				Values::hex_textures[hex->team]
			);
			target.forward(&assets::tilemap);
		};

		// draw water tiles
		if (hex->type == Hex::Water) {
			target.quad({ origin + Values::tileLevel, Values::tileSize }, Values::water);
			target.forward(&assets::tilemap);
		};
	};

	/// Draws tile borders.
	void Tile::drawBorders(ui::RenderBuffer& target, NeighborTest test, sf::Color color) const {
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
			if (test(hex, nb))
				border |= 1 << i;
		};

		// ignore if no borders
		if (!border) return;

		// convert border index into border texture coords
		sf::IntRect texmap = { { (border & 7) * 64, (border >> 3) * 64 }, { 64, 64 } };

		// draw borders
		target.quad({ origin, Values::tileSize }, texmap, color);
		target.forward(&assets::borders);
	};

	/// Draws tile sides.
	void Tile::drawSides(ui::RenderBuffer& target, sf::Color color) const {
		// ignore if not a solid tile
		if (!hex->solid()) return;

		// check if any side can be seen
		const Hex* n2 = map[map.neighbor(coords, Map::LowerRight)];
		const Hex* n3 = map[map.neighbor(coords, Map::LowerLeft)];
		bool visible = (!n2 || !n2->solid()) || (!n3 || !n3->solid());

		// draw border if visible
		if (visible) {
			target.quad({ origin + Values::tileLevel, Values::tileSize }, Values::sides, color);
			target.forward(&assets::tilemap);
		};
	};

	/// Checks whether a neighbor is from a different region.
	bool regionBorderTest(const Hex* origin, const Hex* neighbor) {
		return !neighbor
			|| neighbor->type != Hex::Ground
			|| neighbor->team != origin->team;
	};
};