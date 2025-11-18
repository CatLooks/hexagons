#include "game/draw/troop_draw.hpp"

namespace Draw {
	/// Draws a troop.
	void troopEntity(const Tile& tile, ui::RenderBuffer& buffer) {
		// ignore if no troop
		if (!tile.hex->troop) return;
		const Troop& troop = *tile.hex->troop;

		// draw troop
		buffer.quad({ tile.origin, Values::tileSize }, Values::troop_textures[troop.type]);
		troopStatus(tile.origin, troop, buffer);
		buffer.forward(&assets::tilemap);
	};

	/// Draws troop status bar.
	void troopStatus(sf::Vector2i pos, const Troop& troop, ui::RenderBuffer& buffer) {
		// get troop health bar
		sf::IntRect texture = Values::troop_hp(troop);

		// draw troop health bar
		buffer.quad(
			{
				{
					pos.x + (Values::tileSize.x - texture.size.x * Values::k) / 2,
					pos.y + Values::tileSize.x / 32
				},
				texture.size * Values::k
			},
			texture
		);
	};
};