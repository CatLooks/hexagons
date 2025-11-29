#include "game/map.hpp"
#include "game/draw.hpp"
#include "flags.hpp"

/// Unselected tile coordinates.
const sf::Vector2i Map::unselected = { -1, -1 };

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
	//drawer.select({ 3, 3 }, Values::tileLevel.y);
	std::optional<Draw::Tile> elevated;

	// draw tile geometry
	while (auto tile = drawer.next()) {
		if (drawer.selected(*tile)) {
			tile->drawSides(target, drawer.selected(), sf::Color::Black, sf::Color::White);
			elevated = tile;
		}
		else {
			tile->drawBase(target);
			tile->drawSides(target, drawer.selected(), sf::Color::Black, sf::Color::Black);
		};
	};

	// draw borders
	drawer.reset();
	while (auto tile = drawer.next()) {
		if (!drawer.selected(*tile))
			tile->drawBorders(target, drawer.selected(), sf::Color::Black);
	};

	// draw elevated tile top
	if (elevated) {
		elevated->drawBase(target);
		elevated->drawBorders(target, drawer.selected(), sf::Color::Black);
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