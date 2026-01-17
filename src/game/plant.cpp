#include "game/plant.hpp"
#include "game/logic/skill_helper.hpp"
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
bool Plant::stage_roll(Map* map) {
	// fail if fresh
	if (fresh) return false;

	// check if plant is near water
	float boost = skillf::nearWater(map, pos) ? 2.5f : 1.f;

	// roll chances for different types
	switch (type) {
		case Bush   : return Random::pity(0.33f * boost, stage, 3);
		case Sapling: return Random::pity(0.20f * boost, stage, 5);
		case Tree   : return Random::pity(0.25f * boost, stage, 4);
	};
	return false;
};

/// Rolls a plant spread.
bool Plant::spread_roll(Map* map) {
	// fail if fresh
	if (fresh) return false;

	// check if plant is near water
	float boost = skillf::nearWater(map, pos) ? 3.f : 1.f;

	// roll chances for different types
	switch (type) {
		case Bush : return Random::pity(0.25f * boost, spread, 4);
		case Berry: return Random::pity(0.33f * boost, spread, 3);
		case Tree : return Random::pity(0.20f * boost, spread, 5);
		case Peach: return Random::pity(0.25f * boost, spread, 4);
		case Pine : return Random::chance(0.95f * boost);
	};
	return false;
};

/// Ticks entity state.
void Plant::tick(Map* map) {
	// attempt to advance stage
	if (stage_roll(map))
		type = logic::plant_grow[type];

	// update freshness
	fresh = false;
};