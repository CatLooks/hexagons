#include "game/draw/build_draw.hpp"

namespace Draw {
	/// Draws a building.
	void buildEntity(const Tile& tile, ui::RenderBuffer& buffer) {
		// ignore if no building
		if (!tile.hex->build) return;
		const Build& build = *tile.hex->build;

		// draw troop
		buffer.quad({ tile.origin, Values::tileSize }, Values::build_textures[build.type]);
		buffer.forward(&assets::tilemap);
	};
};