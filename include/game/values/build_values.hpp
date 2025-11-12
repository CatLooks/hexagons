#pragma once
#include "shared.hpp"
#include "game/build.hpp"

namespace Values {
	/// Texture maps for buildings.
	/// @param id Building type.
	extern const sf::IntRect build_textures[Build::Count];

	/// Localization requests for buildings.
	/// @param id Building type.
	extern const char* build_names[Build::Count];
};