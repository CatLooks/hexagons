#pragma once
#include "shared.hpp"
#include "game/map.hpp"

namespace Values {
	/// Water hex.
	extern const sf::IntRect water;
	/// White hex sides.
	extern const sf::IntRect sides;
	/// Hex sides shading.
	extern const sf::IntRect sideShade;

	/// Texture maps for teams.
	/// @param id Team ID.
	extern const sf::IntRect hex_textures[Hex::Count];

	/// Localization requests for teams.
	/// @param id Team ID.
	extern const char* hex_names[Hex::Count];

	/// Colors for teams.
	/// @param id Team ID.
	extern const sf::Color hex_colors[Hex::Count];
};