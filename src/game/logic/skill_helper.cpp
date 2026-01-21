#include "game/logic/skill_helper.hpp"

namespace skillf {
	/// Generates static skill cost generator.
	Skill::Cost cost(int pts) {
		return [=](const SkillState&) {
			return pts;
		};
	};

	/// Generates tile selector spreader effect function.
	Spread::Effect selectTile(size_t idx) {
		return [=](const Spread::Tile& tile) {
			tile.hex->selected = idx;
		};
	};

	/// Allows spreading only to tiles of a region.
	Spread::Check sameRegionHop(const Regions::Ref& region) {
		return [&region](const Spread::Tile& tile) {
			return tile.hex->region() == region;
		};
	};

	/// Allows spreading only to solid tiles.
	const Spread::Check solidHop = [](const Spread::Tile& tile) {
		return tile.hex->solid();
	};

	/// Allows selection only if a tile is empty.
	const Spread::Check emptyPass = [](const Spread::Tile& tile) {
		return tile.hex->free();
	};

	/// Generates region audit spreader effect function.
	Spread::Effect regionJoin(const Regions::Ref& ref) {
		return [&ref](const Spread::Tile& tile) {
			tile.hex->join(ref);
		};
	};

	/// Generates a same region spreader for empty tiles.
	Spread::Check sameRegionEmptyHop(const Regions::Ref& region) {
		return [&region](const Spread::Tile& tile) {
			return tile.hex->region() == region && !tile.hex->entity();
		};
	};

	/// Checks for a test success in a neighborhood.
	size_t checkAround(Map* map, sf::Vector2i pos, size_t radius, Spread::Check check, bool alt) {
		size_t counter = 0;

		// create & apply spread
		Spread spread = {
			.pass = check,
			.effect = [&](const Spread::Tile&) { counter++; },
			.alt = alt
		};
		spread.apply(*map, pos, radius);

		// return test stats
		return counter;
	};

	/// Checks if a tile is near water.
	bool nearWater(Map* map, sf::Vector2i pos) {
		return checkAround(map, pos, 1, [=](const Spread::Tile& nb) {
			return nb.hex->type == Hex::Water;
		}, Spread::Alt);
	};
};