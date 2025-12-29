#include "game/region.hpp"
#include "game/map.hpp"
#include "game/logic/skill_helper.hpp"

/// Adds another region resources.
void RegionRes::add(const RegionRes& oth) {
	money += oth.money;
	berry += oth.berry;
	peach += oth.peach;
};

/// Subtracts another region resources.
void RegionRes::sub(const RegionRes& oth) {
	money -= oth.money;
	berry -= oth.berry;
	peach -= oth.peach;
};

/// Divides region resources rounded down.
RegionRes RegionRes::div(int count) const {
	return {
		.money = (int)roundf((float)money / count),
		.berry = (int)roundf((float)berry / count),
		.peach = (int)roundf((float)peach / count)
	};
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
			map->ats({ x, y }).leave();

	// enumerate region pointers
	for (int y = 0; y < map->size().y; y++) {
		for (int x = 0; x < map->size().x; x++) {
			Hex& hex = map->ats({x, y});

			// ignore tile if already has a region
			// or the tile does not need a region
			if (hex.region() || !hex.solid()) continue;

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
			return tile.hex->region() == prev;
		},
		.effect = [&](Spread::Tile& tile) {
			// overwrite tile region
			tile.hex->join(next);
		},
		.imm = true
	};
};

/// Merges regions into a singular region.
Regions::Split Regions::merge(
	Map* map,
	const Ref& target,
	const std::vector<AccessPoint>& aps,
	int origin
) {
	// previous resource distribution
	Split dist;

	// store target resources
	auto res = static_cast<RegionRes>(*target);

	// overwrite merged regions
	int idx = 0;
	for (const auto& ap : aps) {
		const Ref& apr = *ap.region;

		// ignore if origin
		if (idx++ == origin) {
			dist.push_back(res);
			continue;
		};

		// store & merge resources
		dist.push_back(*apr);
		target->add(*apr);

		// overwrite region for all hexes
		Ref copy = apr;
		_region_overwrite(copy, target).apply(*map, ap.pos);
	};
	return dist;
};

/// Splits a separated region into proper regions.
void Regions::split(Map* map, const std::vector<AccessPoint>& aps, const Split& dist) {
	// ignore if no regions to split
	if (aps.size() < 2) return;

	// resource split per region
	RegionRes split;

	// generate split amount for empty distribution
	if (dist.empty())
		split = (*aps[0].region)->div((int)aps.size());

	// overwrite region parts
	for (size_t i = 1; i < aps.size(); i++) {
		const auto& ap = aps[i];
		const auto& main = *aps[0].region;

		// generate new region
		Ref apr = *ap.region;
		Ref region = create({ .team = apr->team });

		// transfer resource split
		if (i < dist.size()) {
			// follow previous distribution
			region->add(dist[i]);
			main->sub(dist[i]);
		}
		else {
			// generate split amount
			if (i == dist.size())
				split = main->div((int)(aps.size() - i));

			// split an equal amount from main region
			region->add(split);
			main->sub(split);
		};

		// overwrite region for all hexes
		_region_overwrite(apr, region).apply(*map, ap.pos);
	};
};