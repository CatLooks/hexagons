#pragma once

// include dependencies
#include "game/skill.hpp"
#include "game/map.hpp"

/// Skill implementation helper functions.
namespace skillf {
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

	/// Generates region audit spreader effect function.
	///
	/// @param region Target region.
	Spread::Effect regionAuditEffect(const Regions::Ref& region);
};