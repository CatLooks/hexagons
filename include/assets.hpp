#pragma once

// include dependencies
#include "assetload.hpp"

namespace assets {
	/// Default game font.
	extern sf::Font font;

	/// Textures for rendering border combinations.
	extern sf::Texture borders;
	/// Textures used when rendering game map.
	extern sf::Texture tilemap;
	/// Textures used when rendering UI.
	extern sf::Texture interface;
	/// Texture used to demonstrate the map selection.
	extern sf::Texture map_example;
};