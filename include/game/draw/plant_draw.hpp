#pragma once

// include dependencies
#include "game/values/plant_values.hpp"
#include "hex_draw.hpp"

namespace Draw {
	/// Draws a plant.
	///
	/// @param tile Tile draw data.
	/// @param target Target render buffer.
	void plantEntity(const Tile& tile, ui::RenderBuffer& buffer);
};