#include "game/map.hpp"
#include "game/draw.hpp"
#include "flags.hpp"

/// Generates a new selection index.
size_t Map::newSelectionIndex() {
	_selection = true;
	return ++_select_idx;
};
/// Returns current selection index.
size_t Map::getSelectionIndex() const {
	return _select_idx;
};

/// Stops map selection.
void Map::stopSelection() {
	_selection = false;
};
/// Checks if a selection is happening.
bool Map::isSelection() const {
	return _selection;
};

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
	// draw backplane
	target.quad(backplane() - camera, {}, sf::Color(40, 42, 48));
	target.forward(nullptr);

	// calculate drawn area
	// @todo
	sf::IntRect area = { {}, size() };
	sf::Vector2i origin = -camera;

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
		if (tile->hex->elevated()) continue;
		tile->drawBorders(target, Draw::white(tile->hex->region == _region));
	};

	// draw tile contents
	drawer.reset();
	while (auto tile = drawer.next()) {
		if (tile->hex->elevated()) continue;
		tile->drawContents(target);
	};

	// draw tile shading
	if (_selection) {
		drawer.reset();
		while (auto tile = drawer.next()) {
			if (tile->hex->elevated()) continue;
			if (tile->hex->selected != _select_idx)
				tile->drawShade(target);
		};
	};

	// draw elevated tile top & contents
	for (auto& tile : elevated) {
		tile.drawBase(target);
		tile.drawBorders(target, sf::Color::White);
		tile.drawContents(target);
	};
};