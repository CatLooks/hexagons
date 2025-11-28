#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <list>
#include "ui/buffer.hpp"
#include "assets.hpp"
#include "mathext.hpp"
#include "region.hpp"

/// Game map object.
/// Stores array of tiles and lists of all dynamic objects on it.
///
/// Hexes are stored in a 2D array.
/// Every other row is shifted by half a tile.
/// Last hex in every shifted row is ignored.
class Map {
	friend Regions;

public:
	/// Neighbor tile index.
	///   5   0
	///    \ /
	/// 4 - # - 1
	///    / \
	///   3   2
	enum nbi_t {
		UpperRight   = 0, /// Neighbor on upper right.
		Right        = 1, /// Neighbor on right.
		LowerRight   = 2, /// Neighbor on lower right.
		LowerLeft    = 3, /// Neighbor on lower left.
		Left         = 4, /// Neighbor on left.
		UpperLeft    = 5, /// Neighbor on upper left.
	};

	/// Returns neighbor position of a tile.
	///
	/// @param pos Tile position.
	/// @param nbi Neighbor tile index.
	///
	/// @return Neighbor tile position.
	static sf::Vector2i neighbor(sf::Vector2i pos, nbi_t nbi);

	/// Unselected tile coordinates.
	static const sf::Vector2i unselected;

public:


private:
	std::unique_ptr<Hex[]> _tiles; /// Tilemap array.
	sf::Vector2i _size;            /// Map size.

	std::list<Troop> _troops; /// Troop pool.
	std::list<Build> _builds; /// Building pool.
	std::list<Plant> _plants; /// Plant pool.

	std::optional<sf::Vector2i> _select; /// Selected tile position.

	/// Unsafe tile lookup.
	///
	/// @param pos Tile position.
	///
	/// @return Tile reference or `nullptr` if does not exist.
	Hex& ats(sf::Vector2i pos) const;

public:
	/// Condition for a spreader function to affect the tile.
	///
	/// @param pos Checked tile position.
	/// @param hex Checked tile reference.
	using SpreadCheck = std::function<bool(sf::Vector2i pos, const Hex& hex)>;
	/// Applies an effect on a tile affected by spreader.
	///
	/// @param pos Affected tile position.
	/// @param hex Affected tile reference.
	using SpreadEffect = std::function<void(sf::Vector2i pos, Hex& hex)>;

	/// Spreader parameter list container.
	///
	/// Used for `spread()`.
	struct SpreadInfo {
		/// Default spread check.
		///
		/// Returns `true` for any hex.
		static bool default_check(sf::Vector2i, const Hex&);
		/// Default spread effect.
		///
		/// Does nothing.
		static void default_effect(sf::Vector2i, Hex&);

		SpreadCheck block    = default_check;  /// Blocking check (if this fails, spread will not propagate to neighbors).
		SpreadCheck pass     = default_check;  /// Non-blocking check (spread will propagate to neighbors regardless).
		SpreadEffect effect  = default_effect; /// Tile spread effect.
	};

	/// Spreader pass index generator.
	///
	/// @return Unique integer.
	static size_t spread_idx();

	/// Spreads an effect on map tiles.
	///
	/// @param pos Spread origin tile position.
	/// @param info Spread configuration.
	/// @param max Maximum radius of spread (infinite by default).
	///
	/// @return List of all affected tile coordinates.
	std::vector<sf::Vector2i> spread(sf::Vector2i pos, const SpreadInfo& info, size_t radius = 0) const;

public:
	sf::Vector2i camera; /// Map camera.

	/// Checks if a position is within the map.
	///
	/// @param pos Tile position.
	bool contains(sf::Vector2i pos) const;

	/// Returns a reference to a tile at position.
	///
	/// If the position is out-of-bounds, a `nullptr` will be returned.
	///
	/// @param pos Tile position.
	///
	/// @return Tile reference.
	Hex* at(sf::Vector2i pos) const;
	/// Returns a reference to a tile at position.
	///
	/// If the position is out-of-bounds, a `nullptr` will be returned.
	///
	/// @param pos Tile position.
	///
	/// @return Tile reference.
	Hex* operator[](sf::Vector2i pos) const;

public:
	/// Adds a troop to the map.
	///
	/// @param troop Troop object.
	void setTroop(const Troop& troop);
	/// Adds a building to the map.
	///
	/// @param build Building object.
	void setBuild(const Build& build);
	/// Adds a plant to the map.
	///
	/// @param plant Plant object.
	void setPlant(const Plant& plant);

public:
	/// Returns amount of hexes stored.
	size_t tilecount() const;
	/// Generates a copy of the map.
	Map copy() const;

	/// Resizes the map.
	///
	/// Old tiles are preserved.
	///
	/// New tiles are empty by default.
	///
	/// @param rect New map rectangle relative to old rectangle.
	void resize(sf::IntRect rect);

public:
	/// Returns backplane rectangle.
	sf::IntRect backplane() const;

	/// Draws the map.
	///
	/// @param target Render buffer.
	void draw(ui::RenderBuffer& target) const;
};