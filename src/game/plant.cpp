#include "game/plant.hpp"

/// Checks whether a plant is harvestable.
bool Plant::harvestable() const {
	return type == Berry || type == Peach;
};

/// Harvests the plant.
RegionRes Plant::harvest() {
	// harvest berry bush
	if (type == Berry) {
		type = Bush;
		return { .berry = 4 };
	};

	// harvest peach tree
	if (type == Peach) {
		type = Tree;
		return { .peach = 4 };
	};
	
	// no resources
	return {};
};