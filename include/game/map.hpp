#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include "ui/buffer.hpp"
#include "assets.hpp"
#include "mathext.hpp"
#include "array.hpp"

/// Game map object.
/// Stores array of tiles and lists of all dynamic objects on it.
///
/// Hexes are stored in a 2D array.
/// Every other row is shifted by half a tile.
/// Last hex in every shifted row is ignored.
class Map : public HexArray {
	friend Regions;

public:
	/// Unselected tile coordinates.
	static const sf::Vector2i unselected;

private:
	Pool<Troop> _troops; /// Troop pool.
	Pool<Build> _builds; /// Building pool.
	Pool<Plant> _plants; /// Plant pool.

	std::optional<sf::Vector2i> _select; /// Selected tile position.

public:
	sf::Vector2i camera; /// Map camera.

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
	/// Returns backplane rectangle.
	sf::IntRect backplane() const;

	/// Draws the map.
	///
	/// @param target Render buffer.
	void draw(ui::RenderBuffer& target) const;
};