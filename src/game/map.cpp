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

/// Changes the team color of a hex.
void Map::repaintHex(const HexRef& origin, const HexRef& tile) {
	// store previous region
	Regions::Ref prev = tile.hex->region;

	// repaint the tile
	if (tile.hex->region) tile.hex->region->removeTile();
	{
		tile.hex->team = origin.hex->team;
		tile.hex->region = origin.hex->region;
	};
	if (tile.hex->region) tile.hex->region->addTile();

	// ignore merge / split checks if region hasn't changed
	if (tile.hex->region == prev) return;

	// check if a merge is needed
	std::vector<Regions::AP> merged;
	for (int i = 0; i < 6; i++) {
		// get neighbor tile
		sf::Vector2i pos = neighbor(tile.pos, static_cast<nbi_t>(i));
		Hex* hex = at(pos);
		if (!hex) continue;

		// check for the same team but different region
		if (hex->team == tile.hex->team && hex->region != tile.hex->region) {
			// ignore if region is registered
			for (const auto& ap : merged)
				if (ap.region == &hex->region)
					continue;

			// register region for merging
			merged.push_back({ &hex->region, pos });
		};
	};

	// merge adjacent regions
	if (!merged.empty()) {
		regions.merge(this, tile.hex->region, merged);
	};

	// find all separate regions
	std::vector<Regions::AP> splits;
	std::vector<size_t> split_idxs;

	for (int i = 0; i < 6; i++) {
		// get neighbor tile
		sf::Vector2i pos = neighbor(tile.pos, static_cast<nbi_t>(i));
		Hex* hex = at(pos);
		if (!hex) continue;

		// ignore if not the same region as overwritten
		if (hex->region != prev) continue;

		// check if region has been reached already
		bool found = false;
		for (size_t idx : split_idxs) {
			if (hex->spread == idx) {
				found = true;
				break;
			};
		};
		if (found) continue;

		// mark all tiles in region
		Spread spread = {
			.hop = [&](const Spread::Tile& tile) {
				return tile.hex->region == prev;
			},
			.imm = true
		};
		size_t idx = spread.apply(*this, pos);

		// add split data
		splits.push_back({ &hex->region, pos });
		split_idxs.push_back(idx);
	};

	// split regions
	regions.split(this, splits);
};

/// Moves a troop to another tile.
void Map::moveTroop(const HexRef& from, const HexRef& to) {
	// remove entities at destination
	removeEntity(to.hex);

	// repaint destination
	repaintHex(from, to);

	// @todo cross region move check (for mines)

	// move troop
	to.hex->troop = std::move(from.hex->troop);
	from.hex->troop = {};
};

/// Removes any entities from the hex.
void Map::removeEntity(Hex* hex) {
	// remove troop
	if (hex->troop) {
		// update region income
		if (hex->region)
			hex->region->income += logic::troop_upkeep[hex->troop->type];

		// delete troop
		hex->troop = {};
	};

	// remove building
	if (hex->build) {
		// update region income
		if (hex->region)
			hex->region->income += logic::build_upkeep[hex->build->type];

		// delete building
		hex->build = {};
	};

	// remove plant
	if (hex->plant) {
		// update region income
		if (hex->region)
			hex->region->income += logic::plant_upkeep;

		// delete plant
		hex->plant = {};
	};
};

/// Adds a troop to the map.
void Map::setTroop(const Troop& troop) {
	auto hex = at(troop.pos);
	if (hex) {
		removeEntity(hex);

		// add troop
		hex->troop = _troops.add(troop);

		// update region income
		if (hex->region)
			hex->region->income -= logic::troop_upkeep[troop.type];
	};
};
/// Adds a building to the map.
void Map::setBuild(const Build& build) {
	auto hex = at(build.pos);
	if (hex) {
		removeEntity(hex);

		// add building
		hex->build = _builds.add(build);

		// update region income
		if (hex->region)
			hex->region->income -= logic::build_upkeep[build.type];
	};
};
/// Adds a plant to the map.
void Map::setPlant(const Plant& plant) {
	auto hex = at(plant.pos);
	if (hex) {
		removeEntity(hex);

		// add plant
		hex->plant = _plants.add(plant);

		// update region income
		if (hex->region)
			hex->region->income -= logic::plant_upkeep;
	};
};

/// Applies an effect on a troop.
void Map::effectTroop(const HexRef& tile, EffectType effect) {
	// ignore if no troop
	if (!tile.hex->troop) return;

	// add effect to effect list
	tile.hex->troop->addEffect(effect);
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
			if (_selection && tile->hex->selected != _select_idx)
				tile->drawSides(target, Values::dimTint, Values::dimTint);
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

	// draw troop status
	drawer.reset();
	while (auto tile = drawer.next()) {
		Draw::troopBar(*tile, target);
	};
};