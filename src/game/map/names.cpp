#include "game/map/names.hpp"

namespace Name {
	/// Returns the name of a hex team.
	const char* hex(Hex::Team id) {
		const char* table[Hex::Count] = {
			"@!team.v", "@!team.r", "@!team.o",
			"@!team.y", "@!team.g", "@!team.a",
			"@!team.b", "@!team.p", "@!team.m",
		};
		return (id >= 0 && id < Hex::Count)
			? table[id] : "@!team.bad";
	};

	/// Returns the name of a troop type.
	const char* troop(Troop::Type id) {
		const char* table[Troop::Count] = {
			"@!troop.farmer",
			"@!troop.lumber",
			"@!troop.spear",
			"@!troop.archer",
			"@!troop.baron",
			"@!troop.knight",
		};
		return (id >= 0 && id < Troop::Count)
			? table[id] : "@!troop.bad";
	};

	/// Returns the name of a building type.
	const char* build(Build::Type id) {
		const char* table[Build::Count] = {
			"@!build.castle",
			"@!build.farm",
			"@!build.mine",
			"@!build.tower",
			"@!build.beacon",
			"@!build.fort",
		};
		return (id >= 0 && id < Build::Count)
			? table[id] : "@!build.bad";
	};

	/// Returns the name of a plant type.
	const char* plant(Plant::Type id) {
		const char* table[Plant::Count] = {
			"@!plant.bush",
			"@!plant.berry",
			"@!plant.sapling",
			"@!plant.tree",
			"@!plant.peach",
			"@!plant.pine",
			"@!plant.grave",
		};
		return (id >= 0 && id < Plant::Count)
			? table[id] : "@!plant.bad";
	};
};