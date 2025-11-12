#include "game/values/plant_values.hpp"

namespace Values {
	// texture array
	const sf::IntRect plant_textures[Plant::Count] = {
		coords(0, 3), // bush
		coords(1, 3), // berry
		coords(2, 3), // sapling
		coords(3, 3), // tree
		coords(4, 3), // peach
		coords(5, 3), // pine
		coords(6, 3), // grave
	};

	// name array
	const char* plant_names[::Plant::Count] = {
		"@!plant.bush",
		"@!plant.berry",
		"@!plant.sapling",
		"@!plant.tree",
		"@!plant.peach",
		"@!plant.pine",
		"@!plant.grave",
	};
};