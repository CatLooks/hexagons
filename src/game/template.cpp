#include "game/template.hpp"

/// Returns template map size.
sf::Vector2i Template::size() const {
	return _size;
};

/// Returns a span of map tiles.
const std::vector<HexBase>& Template::tiles() const {
	return _tiles;
};

/// Returns hex data at position.
HexBase& Template::at(int x, int y) {
	return _tiles[y * (size_t)_size.x + x];
};
const HexBase& Template::at(int x, int y) const {
	return _tiles[y * (size_t)_size.x + x];
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
			temp.at(x, y) = map->ats({x, y}).base();

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
	// clear the map
	map->clear();
	map->empty(_size);

	// copy tile data
	for (int y = 0; y < _size.y; y++) {
		for (int x = 0; x < _size.x; x++) {
			(HexBase&)map->ats({ x, y }) = at(x, y);
		};
	};

	// instantiate regions
	map->regions.enumerate(map);

	// construct entities
	for (const auto& troop : troops) map->setTroop(troop);
	for (const auto& build : builds) map->setBuild(build);
	for (const auto& plant : plants) map->setPlant(plant);

	// copy region data
	for (const auto& rcd : regions) {
		Hex& hex = map->ats(rcd.pos);
		if (hex.region())
			hex.region()->setRes(rcd.res);
	};
};
