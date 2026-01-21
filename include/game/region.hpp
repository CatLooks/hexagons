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

	/// Returns region resources.
	RegionRes res() const;

	/// Sets new region resources.
	/// 
	/// @param res Region resources.
	void setRes(const RegionRes& res);
};

/// Region variable counters.
struct RegionVar {
	int farms; /// Farm count.
	int tents; /// Tent count.

	/// Returns region variable counters.
	RegionVar var() const;

	/// Sets new region counters.
	/// 
	/// @param var Region counters.
	void setVar(const RegionVar& var);
};

/// Independent region data
struct RegionData : RegionRes, RegionVar {
	bool dead  = 0; /// Whether the region is dead.
	
	/// Returns region data.
	RegionData data() const;

	/// Sets new region data.
	/// 
	/// @param data Region data.
	void setData(const RegionData& data);
};

/// Region statistics object.
struct Region : RegionData {
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

	/// Updates money based on income.
	void tick();
};

// forward declare map
class Map;
namespace gameui { class Loader; };

/// Region manager object.
class Regions {
	friend dev::Factory;
	friend gameui::Loader;

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
	/// Returns a region iterator.
	RefPool<Region>::It iter();

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

	/// Executes the function for each region in a map.
	///
	/// @param map Map reference.
	/// @param call Callback function.
	static void foreach(const Map* map, std::function<void(Region& reg, sf::Vector2i pos)> call);

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