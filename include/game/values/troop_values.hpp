#pragma once
#include "shared.hpp"
#include "game/troop.hpp"

namespace Values {
	/// Texture maps for troops.
	/// @param id Troop type.
	extern const sf::IntRect troop_textures[Troop::Count];

	/// Localization requests for troops.
	/// @param id Troop type.
	extern const char* troop_names[Troop::Count];
};