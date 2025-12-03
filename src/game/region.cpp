#include "game/region.hpp"
#include "game/map.hpp"
#include "game/funcs.hpp"

/// Adds a tile to region.
void Region::addTile() {
	tiles++;
	income++;
};
/// Removes a tile from region.
void Region::removeTile() {
	tiles--;
	income--;
};
/// Updates money based on income.
void Region::tick() {
	money += income;
};

/// Creates a new region.
Regions::Ref Regions::create(Region&& region) {
	return _pool.add(region);
};

/// Enumerates all regions in a map.
void Regions::enumerate(Map* map) {
	// clear region pointers
	for (int y = 0; y < map->size().y; y++)
		for (int x = 0; x < map->size().x; x++)
			map->ats({ x, y }).region = {};

	// enumerate region pointers
	for (int y = 0; y < map->size().y; y++) {
		for (int x = 0; x < map->size().x; x++) {
			Hex& hex = map->ats({x, y});

			// ignore tile if already has a region
			// or the tile does not need a region
			if (hex.region || !hex.solid()) continue;

			// create and spread new region
			auto region = create({ .team = hex.team });
			spreadf::regionCalculation(hex.team, region).apply(*map, { x, y });
		};
	};
};