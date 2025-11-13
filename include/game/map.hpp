#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <list>
#include "ui/buffer.hpp"
#include "assets.hpp"
#include "div.hpp"
#include "hex.hpp"

/// Game map object.
/// Stores array of tiles and lists of all dynamic objects on it.
///
/// Hexes are stored in a 2D array.
/// Every other row is shifted by half a tile.
/// Last hex in every shifted row is ignored.
class Map {
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

private:
	std::unique_ptr<Hex[]> _tiles; /// Tilemap array.
	sf::Vector2i _size;            /// Map size.

	std::list<Troop> _troops; /// Troop pool.
	std::list<Build> _builds; /// Building pool.
	std::list<Plant> _plants; /// Plant pool.

	sf::Vector2i _camera; /// Map camera.

public:
	/// Shifts map camera.
	///
	/// @param offset Camera offset.
	void shiftCamera(sf::Vector2i offset);

public:
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