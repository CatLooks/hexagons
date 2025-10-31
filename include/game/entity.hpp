#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include <stdint.h>

/// Entity object.
/// Stores common data shared between all entities.
struct Entity {
	sf::Vector2i pos; /// Entity position on map.
};