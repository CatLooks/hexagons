#pragma once

// include dependencies
#include "spread.hpp"

/// Spread functions namespace.
namespace spreadf {
	/// Creates a same team spread check.
	/// 
	/// @param team Matched team.
	/// 
	/// @return Spread check function.
	Spread::Check sameTeam(Region::Team team);

	/// Creates a region audit spread effect.
	/// 
	/// @param ref Region reference.
	/// 
	/// @return Spread effect function.
	Spread::Effect regionAudit(const Regions::Ref& ref);

	/// Creates a region recalculation spreader.
	/// 
	/// @param team Target team.
	/// @param ref Region reference.
	/// 
	/// @return Spreader object.
	Spread regionCalculation(Region::Team team, const Regions::Ref& ref);
};