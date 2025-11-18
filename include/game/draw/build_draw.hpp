#pragma once

// include dependencies
#include "game/values/build_values.hpp"
#include "hex_draw.hpp"

namespace Draw {
	/// Draws a building.
	///
	/// @param tile Tile draw data.
	/// @param target Target render buffer.
	void buildEntity(const Tile& tile, ui::RenderBuffer& buffer);
};