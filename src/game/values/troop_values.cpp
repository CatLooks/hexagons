#include "game/values/troop_values.hpp"
#include "mathext.hpp"

namespace Values {
	// texture array
	const sf::IntRect troop_textures[Troop::Count] = {
		coords(0, 1), // farmer
		coords(1, 1), // lumberjack
		coords(2, 1), // spearman
		coords(3, 1), // archer
		coords(4, 1), // baron
		coords(5, 1), // knight
	};

	// name array
	const char* troop_names[Troop::Count] = {
		"@!troop.farmer",
		"@!troop.lumber",
		"@!troop.spear",
		"@!troop.archer",
		"@!troop.baron",
		"@!troop.knight",
	};

	/// Troops available in the region shoop.
	const std::vector<Troop::Type> troop_shop = {
		Troop::Farmer, Troop::Lumberjack, Troop::Spearman,
		Troop::Archer, Troop::Baron, Troop::Knight
	};
};