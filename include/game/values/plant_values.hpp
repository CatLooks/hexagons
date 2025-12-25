#pragma once
#include "game/values/shared.hpp"
#include "game/plant.hpp"

namespace Values {
	/// Texture maps for plants.
	/// @param id Plant type.
	extern const sf::IntRect plant_textures[Plant::Count];

	/// Localization requests for plants.
	/// @param id Plant type.
	extern const char* plant_names[Plant::Count];
};