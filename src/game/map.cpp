#include "game/map.hpp"
#include "game/draw.hpp"
#include "flags.hpp"

/// Selects a region.
void Map::selectRegion(const Regions::Ref& region) {
	if (region) _region = region;
};
/// Deselects a region.
void Map::deselectRegion() {
	_region = {};
};
/// Returns currently selected region.
const Regions::Ref& Map::selectedRegion() const {
	return _region;
};

/// Adds a troop to the map.
void Map::setTroop(const Troop& troop) {
	auto hex = at(troop.pos);
	if (hex) hex->troop = _troops.add(troop);
};
/// Adds a building to the map.
void Map::setBuild(const Build& build) {
	auto hex = at(build.pos);
	if (hex) hex->build = _builds.add(build);
};
/// Adds a plant to the map.
void Map::setPlant(const Plant& plant) {
	auto hex = at(plant.pos);
	if (hex) hex->plant = _plants.add(plant);
};

/// Returns backplane rectangle.
sf::IntRect Map::backplane() const {
	// calculate height
	int y = Values::tileUnit;
	if (size().y > 1)
		y += Values::tileOff.y * (size().y - 1);

	// return backplane rectangle
	return {
		-Values::mapBorder,
		sf::Vector2i(Values::tileOff.x * size().x, y) + Values::mapBorder * 2
	};
};

/// Draws the map.
void Map::draw(ui::RenderBuffer& target) const {
	// center camera
	sf::Vector2i cam = camera - target.screen().size / 2;

	// draw backplane
	target.quad(backplane() - cam, {}, sf::Color(40, 42, 48));
	target.forward(nullptr);

	// calculate drawn area
	// @todo
	sf::IntRect area = { {}, size() };
	sf::Vector2i origin = -cam;

	// setup tile drawer
	TileDrawer drawer(this, area, origin, Values::tileSize);
	std::deque<Draw::Tile> elevated;

	// draw tile geometry
	while (auto tile = drawer.next()) {
		if (tile->hex->elevated()) {
			tile->drawSides(target, sf::Color::White, sf::Color::Black);
			elevated.push_back(*tile);
		}
		else {
			tile->drawBase(target);
			tile->drawSides(target, Draw::white(tile->hex->region == _region), sf::Color::Black);
		};
	};

	// draw borders
	drawer.reset();
	while (auto tile = drawer.next()) {
		if (!tile->hex->elevated())
			tile->drawBorders(target, Draw::white(tile->hex->region == _region));
	};

	// draw elevated tile top
	for (auto& tile : elevated) {
		tile.drawBase(target);
		tile.drawBorders(target, sf::Color::White);
	};

	// draw tile contents
	drawer.reset();
	while (auto tile = drawer.next()) {
		Draw::troopEntity(*tile, target);
		Draw::buildEntity(*tile, target);
		Draw::plantEntity(*tile, target);

		// debug stuff
		if (flags::debug) tile->drawDebug(target);
	};
};