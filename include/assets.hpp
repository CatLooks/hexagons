#pragma once

// include dependencies
#include "assetload.hpp"

namespace assets {
	/// Default game font.
	sf::Font font;

	/// Loads all assets.
	void loadAssets() {
		loadFont(font, "font.ttf");
	};
};