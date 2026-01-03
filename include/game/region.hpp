#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include <refpool>
#include "dev/dev_game.hpp"

/// Region resources.
struct RegionRes {
	int money = 0; /// Amount of money.
	int berry = 0; /// Amount of berries.
	int peach = 0; /// Amount of peaches.

	/// Adds another region resources.
	void add(const RegionRes& oth);

	/// Subtracts another region resources.
	void sub(const RegionRes& oth);

	/// Divides region resources rounded down.
	/// 
	/// @param count Division count.
	/// 
	/// @return New region resource object.
	RegionRes div(int count) const;
};

/// Region statistics object.
struct Region : RegionRes {
	/// Returns region resources.
	RegionRes res() const;

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

	int income = 0; /// Income during next turn.
	int tiles  = 0; /// Amount of tiles captured.
	int farms  = 0; /// Amount of farms built.

	/// Checks whether the region is dead.
	bool dead() const;

	/// Updates money based on income.
	void tick();
};

// forward declare map
class Map;

/// Region manager object.
class Regions {
	friend dev::Factory;

public:
	/// Shared region reference type.
	using Ref = RefPool<Region>::Share;
	/// Region split distribution.
	using Split = std::vector<RegionRes>;

	/// Region access point.
	struct AccessPoint {
		const Ref* region {}; /// Region reference.
		sf::Vector2i  pos {}; /// Access position.
	};

private:
	RefPool<Region> _pool; /// Region pool.

public:
	/// Creates a new region.
	///
	/// @param region Initial region state.
	/// 
	/// @return Region shared reference.
	Ref create(const Region& region);

	/// Enumerates all regions in a map.
	///
	/// @param map Map reference.
	void enumerate(Map* map);

	/// Merges regions into a singular region.
	///
	/// @param map Map reference.
	/// @param target Target region.
	/// @param aps Merged region access points.
	/// @param origin Merge origin access point index.
	/// 
	/// @return Previous resource distribution.
	Split merge(
		Map* map,
		const Ref& target,
		const std::vector<AccessPoint>& aps,
		int origin
	);

	/// Splits a separated region into proper regions.
	///
	/// Regions not described by `dist` will have the remaining resources split equally. 
	/// 
	/// @param map Map reference.
	/// @param aps Access points to all separated region parts.
	/// @param dist Previous resource distribution.
	void split(Map* map, const std::vector<AccessPoint>& aps, const Split& dist);
};