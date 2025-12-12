#include "game/draw/troop_draw.hpp"

namespace Draw {
	/// Draws a troop.
	void troopEntity(const Tile& tile, ui::RenderBuffer& buffer) {
		// ignore if no troop
		if (!tile.hex->troop) return;
		const Troop& troop = *tile.hex->troop;

		// draw troop
		buffer.quad({ tile.origin, Values::tileSize }, Values::troop_textures[troop.type]);
		buffer.forward(&assets::tilemap);
	};
};