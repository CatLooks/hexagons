#include "game/region.hpp"
#include "game/map.hpp"

/// Enumerates all regions in a map.
void Regions::enumerate(Map* map) {
	// clear region pointers
	for (int y = 0; y < map->_size.y; y++)
		for (int x = 0; x < map->_size.x; x++)
			map->ats({x, y}).region = nullptr;

	// enumerate region pointers
	for (int y = 0; y < map->_size.y; y++) {
		for (int x = 0; x < map->_size.x; x++) {
			Hex& hex = map->ats({x, y});

			// ignore tile if already has a region
			if (hex.region) continue;

			// create new region
			
		};
	};
};