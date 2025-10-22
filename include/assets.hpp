#pragma once

// include dependencies
#include "assetload.hpp"

namespace assets {
	/// Default game font.
	sf::Font font;

	/// Tileset texture
	sf::Texture tiles;

	/// Loads all assets.
	void loadAssets() {
		loadFont("font.ttf", font);
		loadTexture("tileset.png", tiles);
	};
};