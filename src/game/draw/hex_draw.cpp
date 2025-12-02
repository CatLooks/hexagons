#include "game/draw/hex_draw.hpp"
#include "game/draw/troop_draw.hpp"
#include "game/draw/build_draw.hpp"
#include "game/draw/plant_draw.hpp"
#include "flags.hpp"

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
				{ origin, size },
				Values::hex_textures[hex->team]
			);
			target.forward(&assets::tilemap);
		};

		// draw water tiles
		if (hex->type == Hex::Water) {
			target.quad({ origin + Values::tileLevel(size), size }, Values::water);
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

		// set every border if selected
		if (hex->elevated()) {
			border = 0x3F;
		}

		// check neighbors
		else for (int i = 0; i < 6; i++) {
			// fetch neighbor hex
			sf::Vector2i pos = map->neighbor(coords, static_cast<Map::nbi_t>(i));
			const Hex* nb = map->at(pos);

			// set border if passes check
			if (!nb || nb->type != Hex::Ground || nb->team != hex->team || nb->elevated())
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
		target.quad({ origin, size }, texmap, color);
		target.forward(&assets::borders);
	};

	/// Draws tile sides.
	void Tile::drawSides(ui::RenderBuffer& target, sf::Color up, sf::Color low) const {
		// check if any side can be seen
		const Hex* n2 = map->at(map->neighbor(coords, Map::LowerRight));
		const Hex* n3 = map->at(map->neighbor(coords, Map::LowerLeft));
		bool visible = hex->elevated() || (!n2 || !n2->solid()) || (!n3 || !n3->solid());

		// draw border if visible
		if (visible) {
			if (hex->solid()) {
				sf::IntRect area = { origin + Values::tileLevel(size), size };

				// draw bottom ground sides
				if (hex->elevated()) {
					target.quad(
						area + sf::Vector2i(sf::Vector2f(Values::tileLevel(size)) * hex->elevation),
						Values::sides, low
					);
				};

				// draw top ground sides
				target.quad(area, Values::sides, up);
				if (up != low) {
					target.quad(area, Values::sideShade, low);
				};
			}
			else if (hex->type == Hex::Water) {
				// draw water sides
				sf::IntRect area = {origin + Values::tileLevel(size) * 2, size};
				target.quad(area, Values::sides, sf::Color(0, 157, 251));
			};
			target.forward(&assets::tilemap);
		};
	};

	/// Draws unselected tile shading.
	void Tile::drawShade(ui::RenderBuffer& target) const {
		// shade only ground tiles
		if (hex->type == Hex::Ground) {
			target.quad({ origin, size }, Values::mask, Values::dimTint);
			target.forward(&assets::tilemap);
		};
	};

	/// Draws debug stuff.
	void Tile::drawDebug(ui::RenderBuffer& target) const {
		// region index text
		std::string label = hex->region ? std::format("{}", hex->region.index()) : "x";
		sf::Text text(assets::font, label, 20);
		text.setPosition((sf::Vector2f)origin + sf::Vector2f(size.x * 3.f / 4, size.y / 2.f));
		text.setOutlineThickness(2);

		// draw index
		target.text(text);
		target.forward(nullptr);
	};

	/// Draws all tile contents.
	void Tile::drawContents(ui::RenderBuffer& target) const {
		// draw entities
		Draw::troopEntity(*this, target);
		Draw::buildEntity(*this, target);
		Draw::plantEntity(*this, target);

		// draw debug stuff
		if (flags::debug) drawDebug(target);
	};
};