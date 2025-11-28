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
	extern const sf::IntRect hex_textures[Region::Count];

	/// Localization requests for teams.
	/// @param id Team ID.
	extern const char* hex_names[Region::Count];

	/// Colors for teams.
	/// @param id Team ID.
	extern const sf::Color hex_colors[Region::Count];
};