#pragma once

// include dependencies
#include <list>

/// Region statistics object.
struct Region {
	/// Region team color.
	enum Team {
		/// (0) No owner.
		/// Can also be used as "void" team to simplify code.
		Unclaimed = 0,

		Red,    /// (1) Red team.
		Orange, /// (2) Orange team.
		Yellow, /// (3) Yellow team.
		Green,  /// (4) Green team.
		Aqua,   /// (5) Aqua team.
		Blue,   /// (6) Blue team.
		Purple, /// (7) Purple team.
		Pink,   /// (8) Pink team.

		Count   /// Team count.
	} team = Unclaimed;

	int money  = 0; /// Amount of money.
	int berry  = 0; /// Amount of berries.
	int peach  = 0; /// Amount of peaches.
	int income = 0; /// Income during next turn.
	int tiles  = 0; /// Amount of tiles captured.
	int farms  = 0; /// Amount of farms built.
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