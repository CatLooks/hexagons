#pragma once

// include dependencies
#include "hex.hpp"
#include <list>

/// Region statistics object.
struct Region {
	int money  = 0; /// Amount of money.
	int berry  = 0; /// Amount of berries.
	int peach  = 0; /// Amount of peaches.
	int income = 0; /// Income during next turn.
	int tiles  = 0; /// Amount of tiles captured.
	int farms  = 0; /// Amount of farms built.

	Hex::Team team = Hex::Unclaimed; /// Region team.
};

// forward declare map
class Map;

/// Region manager object.
class Regions {
private:
	std::list<Region> _list; /// Region list.

	/// 
	Region* create();

public:
	/// Enumerates all regions in a map.
	///
	/// @param map Map reference.
	void enumerate(Map* map);
};