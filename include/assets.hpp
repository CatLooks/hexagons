#pragma once

// include dependencies
#include "assetload.hpp"

namespace assets {
	/// Default game font.
	sf::Font font;

	/// Test texture
	sf::Texture tex;

	/// Loads all assets.
	void loadAssets() {
		loadFont("font.ttf", font);
		loadTexture("panel.png", tex);
	};
};