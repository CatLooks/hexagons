#include "game/region.hpp"
#include "game/map.hpp"
#include "game/logic/skill_helper.hpp"

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
Regions::Ref Regions::create(const Region& region) {
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
			Spread spread = {
				.hop = [=, team = hex.team](const Spread::Tile& tile) {
					// hop if same team and a ground tile
					return tile.hex->team == team
						&& tile.hex->solid();
				},
				.effect = skillf::regionAuditEffect(region),
				.imm = true
			};
			spread.apply(*map, { x, y });
		};
	};
};

/// Creates a region overwrite spreader object.
///
/// @param prev Previous region to overwrite.
/// @param next New region to overwrite with.
static Spread _region_overwrite(const Regions::Ref& prev, const Regions::Ref& next) {
	return Spread {
		.hop = [&](const Spread::Tile& tile) {
			// hop if same region
			return tile.hex->region == prev;
		},
		.effect = [&](Spread::Tile& tile) {
			// overwrite tile region
			tile.hex->region = next;
		},
		.imm = true
	};
};

/// Merges regions into a singular region.
void Regions::merge(Map* map, Ref& target, const std::vector<AccessPoint>& aps) {
	// overwrite merged regions
	for (const auto& ap : aps) {
		const Ref& apr = *ap.region;

		// merge resources to the target region
		// @todo

		// overwrite region for all hexes
		Ref copy = apr;
		_region_overwrite(copy, target).apply(*map, ap.pos);
	};
};

/// Splits a separated region into proper regions.
void Regions::split(Map* map, const std::vector<AccessPoint>& aps) {
	// overwrite region parts
	for (size_t i = 1; i < aps.size(); i++) {
		const auto& ap = aps[i];

		// generate new region
		Ref apr = *ap.region;
		Ref region = create(*apr);

		// split resources to a new region
		// @todo

		// overwrite region for all hexes
		_region_overwrite(apr, region).apply(*map, ap.pos);
	};
};