#include "game/draw/plant_draw.hpp"

namespace Draw {
	/// Draws a plant.
	void plantEntity(const Tile& tile, ui::RenderBuffer& buffer) {
		// ignore if no building
		if (!tile.hex->plant) return;
		const Plant& plant = *tile.hex->plant;

		// draw troop
		buffer.quad({ tile.origin, Values::tileSize }, Values::plant_textures[plant.type]);
		buffer.forward(&assets::tilemap);
	};
};