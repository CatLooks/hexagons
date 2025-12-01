#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include "ui/buffer.hpp"
#include "assets.hpp"
#include "mathext.hpp"
#include "array.hpp"
#include "spread.hpp"

/// Game map object.
/// Stores array of tiles and lists of all dynamic objects on it.
///
/// Hexes are stored in a 2D array.
/// Every other row is shifted by half a tile.
/// Last hex in every shifted row is ignored.
class Map : public HexArray {
	friend Regions;

public:
	sf::Vector2i camera; /// Map camera.

private:
	Pool<Troop> _troops; /// Troop pool.
	Pool<Build> _builds; /// Building pool.
	Pool<Plant> _plants; /// Plant pool.

	Regions::Ref _region; /// Selected region reference.

public:
	Regions regions; /// Region manager.

	/// Selects a region.
	/// 
	/// @param region Region reference.
	void selectRegion(const Regions::Ref& region);
	/// Deselects a region.
	void deselectRegion();
	/// Returns currently selected region.
	const Regions::Ref& selectedRegion() const;

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