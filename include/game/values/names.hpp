#pragma once

// include dependencies
#include "game/hex.hpp"

/// Name resolution namespace.
namespace Name {
	/// Returns the name of a hex team.
	///
	/// @param id Hex team index.
	///
	/// @return Team name localization path.
	const char* hex(Hex::Team id);

	/// Returns the name of a troop type.
	///
	/// @param id Troop type.
	///
	/// @return Troop localization path.
	const char* troop(Troop::Type id);

	/// Returns the name of a building type.
	///
	/// @param id Building type.
	///
	/// @return Building localization path.
	const char* build(Build::Type id);

	/// Returns the name of a plant type.
	///
	/// @param id Plant type.
	///
	/// @return Plant localization path.
	const char* plant(Plant::Type id);
};