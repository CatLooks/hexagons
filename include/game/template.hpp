#pragma once

// include dependencies
#include "map.hpp"

/// Game state template.
///
/// Stores all info to reconstruct the map.
class Template {
private:
	sf::Vector2i _size;
	std::vector<HexBase> _tiles;

public:
	/// Returns template map size.
	sf::Vector2i size() const;
	/// Returns a span of map tiles.
	const std::vector<HexBase>& tiles() const;

	/// Clears and creates a new empty map.
	///
	/// @param size New map size.
	void clear(sf::Vector2i size);

public:
	/// Region construction data.
	struct RegionData {
		RegionRes    res; /// Region resources.
		sf::Vector2i pos; /// Region access point.
	};

	std::vector<Troop> troops; /// Troop list.
	std::vector<Build> builds; /// Build list.
	std::vector<Plant> plants; /// Plant list.

	/// Region resource list.
	std::vector<RegionData> regions;

	/// Generates a template from the map.
	///
	/// @param map Map reference.
	///
	/// @return New template object.
	static Template generate(const Map* map);

	/// Constructs the map based on the template.
	///
	/// @param map Map reference.
	void construct(Map* map) const;
};