#include "game/map.hpp"
#include "game/draw.hpp"
#include "flags.hpp"

/// Constructs an empty game map.
Map::Map() : history(this) {};

/// Returns troop iterator.
Pool<Troop>::It Map::troopList() { return _troops.iter(); };
/// Returns troop iterator.
Pool<Build>::It Map::buildList() { return _builds.iter(); };
/// Returns troop iterator.
Pool<Plant>::It Map::plantList() { return _plants.iter(); };

/// Returns troop iterator.
Pool<Troop>::ConstIt Map::troopList() const { return _troops.iter(); };
/// Returns troop iterator.
Pool<Build>::ConstIt Map::buildList() const { return _builds.iter(); };
/// Returns troop iterator.
Pool<Plant>::ConstIt Map::plantList() const { return _plants.iter(); };

/// Clears the map.
void Map::clear() {
	// clear tiles
	HexArray::clear();

	// reset indices
	Spread::reset();
	_select_idx = 0;

	// reset other stuff
	history.clear();
	_selection = false;
	_region = {};
	pulse = {};
};

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

/// Checks for region splits / joins.
Regions::Split Map::updateRegions(
	const HexRef& tile,
	const Regions::Ref& prev,
	const Regions::Split& split
) {
	/// ==== merge ==== ///

	// merged regions list
	std::vector<Regions::AccessPoint> merged;
	int origin = -1;

	// check if a merge is needed
	for (int i = 0; i < 6; i++) {
		// get neighbor tile
		sf::Vector2i pos = neighbor(tile.pos, static_cast<nbi_t>(i));
		Hex* hex = at(pos);
		if (!hex) continue;

		// check for the same team
		if (hex->team == tile.hex->team) {
			// but different region
			if (hex->region() != tile.hex->region()) {
				// ignore if region has already been recorded
				bool match = false;
				for (const auto& ap : merged) {
					if (hex->region() == *ap.region) {
						match = true;
						break;
					};
				};

				// store region access point
				if (!match)
					merged.push_back({ &hex->region(), pos });
			}
			else if (origin < 0) {
				// record merge origin
				origin = (int)merged.size();

				// store region access point
				merged.push_back({ &hex->region(), pos });
			};
		};
	};

	// merge regions if needed
	auto dist = regions.merge(this, tile.hex->region(), merged, origin);

	/// ==== split ==== ///

	// split regions list
	std::vector<Regions::AccessPoint> splits;
	std::vector<size_t> indices; // spread indices for each AP

	// find all separated regions
	for (int i = 0; i < 6; i++) {
		// get neighbor tile
		sf::Vector2i pos = neighbor(tile.pos, static_cast<nbi_t>(i));
		Hex* hex = at(pos);
		if (!hex) continue;

		// ignore if not overwritten region
		if (hex->region() != prev) continue;

		// ignore if region has been reached
		bool found = false;
		for (size_t idx : indices) {
			if (hex->spread[Spread::Def] == idx) {
				found = true;
				break;
			};
		};
		if (found) continue;

		// mark all tiles in region
		Spread spread = {
			.hop = [&](const Spread::Tile& tile) {
				return tile.hex->region() == prev;
			},
			.imm = true
		};
		size_t idx = spread.apply(*this, pos);

		// store region access point
		splits.push_back({ &hex->region(), pos});
		indices.push_back(idx);
	};

	// split regions if needed
	regions.split(this, splits, split);

	// return merge distribution
	return dist;
};

/// Removes any entities from the hex.
void Map::removeEntity(Hex* hex) {
	// remove troop
	if (hex->troop) {
		// update region income
		if (hex->region())
			hex->region()->income += logic::troop_upkeep[hex->troop->type];

		// delete troop
		hex->troop = {};
	};

	// remove building
	if (hex->build) {
		// update region income
		if (hex->region()) {
			hex->region()->income += logic::build_upkeep[hex->build->type];

			// update farm count
			if (hex->build->type == Build::Farm) hex->region()->farms--;
			// update tent count
			if (hex->build->type == Build::Tent) hex->region()->tents--;
		};

		// delete building
		hex->build = {};
	};

	// remove plant
	if (hex->plant) {
		// update region income
		if (hex->region())
			hex->region()->income += logic::plant_upkeep;

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
		if (hex->region())
			hex->region()->income -= logic::troop_upkeep[troop.type];
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
		if (hex->region()) {
			hex->region()->income -= logic::build_upkeep[build.type];

			// update farm count
			if (hex->build->type == Build::Farm) hex->region()->farms++;
			// update tent count
			if (hex->build->type == Build::Tent) hex->region()->tents++;
		};
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
		if (hex->region())
			hex->region()->income -= logic::plant_upkeep;
	};
};

/// Executes a move.
void Map::executeSkill(Move* move, sf::Vector2i pos, const Skill* skill) {
	// ignore if move is not instantiated
	if (!move) return;

	// attach skill info to the move
	move->skill_pos = pos;
	move->skill_type = skill->type;
	move->skill_cooldown = skill->cooldown;

	// add move to history
	history.add(move);
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
void Map::draw(ui::RenderBuffer& target, float t) const {
	// draw backplane
	target.quad(backplane() - camera.position, {}, sf::Color(40, 42, 48));
	target.forward(nullptr);

	// get top-left corner tile position
	auto [tx, mx] = ext::idivmod(camera.position.x, Values::tileOff.x);
	auto [ty, my] = ext::idivmod(camera.position.y, Values::tileOff.y);

	// shift corner further
	tx--; mx += Values::tileOff.x;
	ty--; my += Values::tileOff.y;

	// get map view size in tiles
	sf::Vector2i view = camera.size.componentWiseDiv(Values::tileOff);

	// calculate drawn area
	sf::IntRect area = { { tx, ty }, view + sf::Vector2i(4, 4) };
	sf::Vector2i origin = { -mx, -my };

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
			tile->drawSides(target, Draw::white(tile->hex->region() == _region), sf::Color::Black);
			if (_selection && tile->hex->selected != _select_idx)
				tile->drawSides(target, Values::dimTint, Values::dimTint);
		};
	};

	// draw borders
	drawer.reset();
	while (auto tile = drawer.next()) {
		if (tile->hex->elevated()) continue;
		tile->drawBorders(target, Draw::white(tile->hex->region() == _region));
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

	// draw tile pulse
	if (pulse) {
		auto tile = drawer.at(*pulse, 1.f);
		tile.drawPulse(target, t);
	};

	// draw elevated tile top & contents
	for (auto& tile : elevated) {
		tile.drawBase(target);
		tile.drawBorders(target, sf::Color::White);
		tile.drawContents(target);
	};

	// draw entity status
	drawer.reset();
	while (auto tile = drawer.next()) {
		Draw::entityBar(*tile, target);
	};
};