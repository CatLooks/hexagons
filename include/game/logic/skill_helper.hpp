#pragma once

// include dependencies
#include "game/skill.hpp"
#include "game/map.hpp"

/// Skill implementation helper functions.
namespace skillf {
	/// Generates static skill cost generator.
	/// 
	/// @param pts Currency amount.
	Skill::Cost cost(int pts);

	/// Generates tile selector spreader effect function.
	/// 
	/// @param idx Selection index.
	Spread::Effect selectTile(size_t idx);

	/// Allows spreading only to tiles of a region.
	/// 
	/// @param region Target region.
	Spread::Check sameRegionHop(const Regions::Ref& region);

	/// Allows spreading only to solid tiles.
	extern const Spread::Check solidHop;

	/// Allows selection only if a tile is empty.
	extern const Spread::Check emptyPass;

	/// Generates region join effect function.
	///
	/// @param region Target region.
	Spread::Effect regionJoin(const Regions::Ref& region);

	/// Generates a same region spreader for empty tiles.
	/// 
	/// @param region Target region.
	Spread::Check sameRegionEmptyHop(const Regions::Ref& region);

	/// Checks for a test success in a neighborhood.
	/// 
	/// @param map Map reference.
	/// @param pos Neighborhood center.
	/// @param radius Neighborhood radius.
	/// @param check Test function.
	/// @param alt Whether to use alternative spread index.
	/// 
	/// @return Amount of tests passed.
	size_t checkAround(
		Map* map,
		sf::Vector2i pos,
		size_t radius,
		Spread::Check check,
		bool alt
	);
};