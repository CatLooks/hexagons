#pragma once

// include dependencies
#include "game/values/troop_values.hpp"
#include "hex_draw.hpp"

namespace Draw {
	/// Draws a troop.
	///
	/// @param tile Tile draw data.
	/// @param target Target render buffer.
	void troopEntity(const Tile& tile, ui::RenderBuffer& buffer);

	/// Draws troop status bar.
	///
	/// @param tile Tile draw data.
	/// @param target Target render buffer.
	void troopStatus(const Tile& tile, ui::RenderBuffer& buffer);
};