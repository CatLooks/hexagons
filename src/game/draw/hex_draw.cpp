#include "game/draw/hex_draw.hpp"

namespace Draw {
	/// Returns a white or a black color, depending on the input.
	sf::Color white(bool enable) {
		return enable ? sf::Color::White : sf::Color::Black;
	};

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
	void Tile::drawBorders(ui::RenderBuffer& target, sf::Vector2i select, sf::Color color) const {
		// ignore if not a ground tile
		if (hex->type != Hex::Ground) return;

		// border index
		// each bit corresponds to a border
		uint8_t border = 0;

		// set every border if selected
		if (coords == select) {
			border = 0x3F;
		}

		// check neighbors
		else for (int i = 0; i < 6; i++) {
			// fetch neighbor hex
			sf::Vector2i pos = map->neighbor(coords, static_cast<Map::nbi_t>(i));
			const Hex* nb = map->at(pos);

			// set border if passes check
			if (!nb || nb->type != Hex::Ground || nb->team != hex->team || pos == select)
				border |= 1 << i;
		};

		// ignore if no borders
		if (!border) return;

		// convert border index into border texture coords
		sf::IntRect texmap = {
			{ (border & 7) * Values::tileTex.x, (border >> 3) * Values::tileTex.y },
			Values::tileTex
		};

		// draw borders
		target.quad({ origin, Values::tileSize }, texmap, color);
		target.forward(&assets::borders);
	};

	/// Draws tile sides.
	void Tile::drawSides(ui::RenderBuffer& target, sf::Vector2i select, sf::Color up, sf::Color low) const {
		// ignore if not a solid tile
		if (!hex->solid()) return;

		// check if any side can be seen
		const Hex* n2 = map->at(map->neighbor(coords, Map::LowerRight));
		const Hex* n3 = map->at(map->neighbor(coords, Map::LowerLeft));
		bool visible = coords == select || (!n2 || !n2->solid()) || (!n3 || !n3->solid());

		// draw border if visible
		if (visible) {
			sf::IntRect area = { origin + Values::tileLevel, Values::tileSize };
			target.quad(area, Values::sides, up);
			if (up != low)
				target.quad(area, Values::sideShade, low);
			target.forward(&assets::tilemap);
		};
	};
};