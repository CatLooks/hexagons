#pragma once

// include dependencies
#include <SFML/Graphics/Rect.hpp>
#include "game/hex.hpp"

/// Texture map resolution namespace.
namespace Textures {
	/// Missing texture replacement.
	extern const sf::IntRect missing;

	/// Water hex texture map.
	extern const sf::IntRect hexWater;

	/// Returns the texture map of a hex team.
	///
	/// @param id Hex team index.
	///
	/// @return Team name texture map.
	sf::IntRect hex(Hex::Team id);
	/// Returns the texture map of a troop type.
	///
	/// @param id Troop type.
	///
	/// @return Troop texture map.
	sf::IntRect troop(Troop::Type id);
	/// Returns the texture map of a building type.
	///
	/// @param id Building type.
	///
	/// @return Building texture map.
	sf::IntRect build(Build::Type id);
	/// Returns the texture map of a plant type.
	///
	/// @param id Plant type.
	///
	/// @return Plant texture map.
	sf::IntRect plant(Plant::Type id);
};