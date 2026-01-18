#include "game/template.hpp"

/// Returns template map size.
sf::Vector2i Template::size() const {
	return _size;
};

/// Returns a span of map tiles.
const std::vector<HexBase>& Template::tiles() const {
	return _tiles;
};

/// Clears and creates a new empty map.
void Template::clear(sf::Vector2i size) {
	_size = size;
	_tiles = std::vector<HexBase>(static_cast<size_t>(_size.x) * static_cast<size_t>(_size.y), HexBase{});
};

/// Generates a template from the map.
Template Template::generate(const Map* map) {
	// create template
	Template temp;

	// copy tile data
	sf::Vector2i size = map->size();
	temp.clear(size);
	for (int y = 0; y < size.y; y++)
		for (int x = 0; x < size.x; x++)
			temp._tiles[y * size.x + x] = map->ats({ x, y });

	// copy entity data
	{
		auto it = map->troopList();
		while (auto* next = it.next())
			temp.troops.push_back(*next);
	};
	{
		auto it = map->buildList();
		while (auto* next = it.next())
			temp.builds.push_back(*next);
	};
	{
		auto it = map->plantList();
		while (auto* next = it.next())
			temp.plants.push_back(*next);
	};

	// copy region data
	Regions::foreach(map, [&temp](Region& reg, sf::Vector2i pos) {
		temp.regions.push_back({ reg.res(), pos });
	});

	// return template
	return temp;
};

/// Constructs the map based on the template.
void Template::construct(Map* map) const {
	printf("not implemented\n");
};