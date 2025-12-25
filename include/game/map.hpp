#pragma once

// include dependencies
#include <SFML/System/Vector2.hpp>
#include "ui/buffer.hpp"

#include "assets.hpp"
#include "mathext.hpp"
#include "array.hpp"
#include "spread.hpp"
#include "history.hpp"

#include "logic/troop_logic.hpp"
#include "logic/build_logic.hpp"
#include "logic/plant_logic.hpp"

/// Game map object.
/// Stores array of tiles and lists of all dynamic objects on it.
///
/// Hexes are stored in a 2D array.
/// Every other row is shifted by half a tile.
/// Last hex in every shifted row is ignored.
class Map : public HexArray {
	friend Regions;
	friend Move;

public:
	/// Constructs an empty game map.
	Map();

	sf::Vector2i camera; /// Map camera.

	/// Generates a new selection index.
	/// 
	/// All tiles with a matching selection index will be highlighted.
	/// 
	/// @return Non-zero selection index.
	size_t newSelectionIndex();
	/// Returns current selection index.
	size_t getSelectionIndex() const;

	/// Stops map selection.
	void stopSelection();
	/// Checks if a selection is happening.
	bool isSelection() const;

private:
	Pool<Troop> _troops; /// Troop pool.
	Pool<Build> _builds; /// Building pool.
	Pool<Plant> _plants; /// Plant pool.

	Regions::Ref _region;    /// Selected region reference.
	size_t _select_idx = 0;  /// Selection index.
	bool _selection = false; /// Whether a selection is happening.

public:
	/// Move history.
	History history;
	/// Region manager.
	Regions regions;
	/// Tile pulse annotation.
	std::optional<sf::Vector2i> pulse;

	/// Selects a region.
	/// 
	/// @param region Region reference.
	void selectRegion(const Regions::Ref& region);
	/// Deselects a region.
	void deselectRegion();
	/// Returns currently selected region.
	const Regions::Ref& selectedRegion() const;

public:
	/// Changes the team color of a hex.
	///
	/// @param origin Repaint source tile.
	/// @param tile Target hex reference.
	void repaintHex(const HexRef& origin, const HexRef& tile);
	/// Moves a troop to another tile.
	///
	/// @param from Source tile.
	/// @param to Destination tile.
	void moveTroop(const HexRef& from, const HexRef& to);

	/// Removes any entities from the hex.
	///
	/// @param hex Hex reference.
	void removeEntity(Hex* hex);

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

	/// Applies an effect on a troop.
	/// 
	/// @param tile Troop tile.
	/// @param effect Effect type.
	/// 
	/// @return Move object.
	Move* effectTroop(const HexRef& tile, EffectType effect);

	/// Executes a skill.
	/// 
	/// @param move Move object.
	/// @param pos Skill origin position.
	/// @param skill Skill description.
	void executeSkill(Move* move, sf::Vector2i pos, const Skill* skill);

public:
	/// Returns backplane rectangle.
	sf::IntRect backplane() const;

	/// Draws the map.
	///
	/// @param target Render buffer.
	/// @param t Pulsing progression parameter.
	void draw(ui::RenderBuffer& target, float t) const;
};