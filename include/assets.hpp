#pragma once

// include dependencies
#include "assetload.hpp"

namespace assets {
	/// Default game font.
	sf::Font font;

	/// Textures for rendering border combinations.
	sf::Texture borders;
	/// Textures used when rendering game map.
	sf::Texture tilemap;
	/// Textures used when rendering UI.
	sf::Texture interface;

	/// Loads all assets.
	void loadAssets() {
		// load fonts
		loadFont("font.ttf", font);

		// load textures
		loadTexture("tilemap.png", tilemap);
		loadTexture("interface.png", interface);
		loadTexture("borders.png", borders);
	};
};