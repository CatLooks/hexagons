#include "game/plant.hpp"
#include "game/logic/plant_logic.hpp"
#include "random.hpp"

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

/// Rolls a stage advancement.
bool Plant::stage_roll() {
	if (fresh) return false;
	switch (type) {
		case Bush   : return Random::pity(0.33f, stage, 3);
		case Sapling: return Random::pity(0.20f, stage, 5);
		case Tree   : return Random::pity(0.25f, stage, 4);
	};
	return false;
};

/// Rolls a plant spread.
bool Plant::spread_roll() {
	if (fresh) return false;
	switch (type) {
		case Bush : return Random::pity(0.25f, spread, 4);
		case Berry: return Random::pity(0.33f, spread, 3);
		case Tree : return Random::pity(0.20f, spread, 5);
		case Peach: return Random::pity(0.25f, spread, 4);
		case Pine : return Random::chance(0.75f);
	};
	return false;
};

/// Ticks entity state.
void Plant::tick() {
	// attempt to advance stage
	if (stage_roll())
		type = logic::plant_grow[type];

	// update freshness
	fresh = false;
};