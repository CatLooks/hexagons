#include "game/plant.hpp"

namespace Name {
	/// Returns a request value for plant name.
	const char* plant(Plant::Type id) {
		static const char* table[Plant::Count] = {
			"@!plant.bush",
			"@!plant.berry",
			"@!plant.sapling",
			"@!plant.tree",
			"@!plant.peach",
			"@!plant.pine",
			"@!plant.grave"
		};
		return (id >= 0 && id < Plant::Count)
			? table[id] : "@!plant.bad";
	};
	/// Returns a request value for plant name.
	const char* plant(const Plant& entity) {
		return plant(entity.type);
	};
};