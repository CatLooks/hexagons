#include "game/values/build_values.hpp"

namespace Values {
	// texture array
	const sf::IntRect build_textures[Build::Count] = {
		coords(0, 2), // town hall
		coords(1, 2), // farm
		coords(2, 2), // mine
		coords(3, 2), // tower
		coords(4, 2), // beacon
		coords(5, 2), // fort
		coords(2, 2), // tent
	};

	// name array
	const char* build_names[Build::Count] = {
		"@!build.castle",
		"@!build.farm",
		"@!build.mine",
		"@!build.tower",
		"@!build.beacon",
		"@!build.fort",
		"@!build.tent"
	};

	/// Buildings available in the region shop.
	const std::vector<Build::Type> build_shop = {
		Build::Farm, Build::Tower, Build::Beacon
	};
};