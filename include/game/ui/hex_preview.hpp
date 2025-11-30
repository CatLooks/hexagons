#pragma once

// include dependencies
#include "ui.hpp"
#include "game.hpp"

/// Stuff used for hex preview.
namespace gameui::preview {
	extern Map map;         /// Preview "map".
	extern Draw::Tile tile; /// Preview tile draw data.

	/// Draws the preview tile.
	///
	/// @param target Render target.
	/// @param self Draw area.
	void draw(ui::RenderBuffer& target, sf::IntRect self);
};