#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include <refpool>

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

	/// Adds a tile to region.
	void addTile();
	/// Removes a tile from region.
	void removeTile();
	/// Updates money based on income.
	void tick();
};

// forward declare map
class Map;

/// Region manager object.
class Regions {
public:
	/// Shared region reference type.
	using Ref = RefPool<Region>::Share;

	/// Region access point.
	struct AP {
		Ref* region      {}; /// Region reference.
		sf::Vector2i pos {}; /// Access position.
	};

private:
	RefPool<Region> _pool; /// Region pool.

	/// Creates a new region.
	///
	/// @param region Initial region state.
	/// 
	/// @return Region shared reference.
	Ref create(const Region& region);

public:
	/// Enumerates all regions in a map.
	///
	/// @param map Map reference.
	void enumerate(Map* map);

	/// Merges other regions with this region.
	/// 
	/// @param map Map reference.
	/// @param region Target region.
	/// @param aps List of merged regions' access points.
	void merge(Map* map, Ref& region, const std::vector<AP>& aps);

	void split(Map* map, const std::vector<AP>& aps);
};