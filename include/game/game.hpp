#pragma once

// include dependencies
#include "ui/layer.hpp"
#include "ui/camera.hpp"
#include "ui/window.hpp"
#include "ui/anim/linear.hpp"
#include "map.hpp"
#include "ui/game_panel.hpp"
#include "ui/hex_preview.hpp"

/// Game controller object.
class Game : public ui::Element {
public:
	Map map; /// Game map.

private:
	std::optional<sf::Vector2i> _select; /// Selected tile.
	ui::Layer*                   _layer; /// Render layer.
	ui::Camera                  _camera; /// Map camera.
	gameui::Panel*               _panel; /// Game panel.

public:
	/// Constructs a game object.
	///
	/// @param layer Render layer.
	/// @param panel Game control panel.
	Game(ui::Layer* layer, gameui::Panel* panel);

	/// Selects a tile at position.
	/// 
	/// @param pos Tile position.
	void selectTile(sf::Vector2i pos);
	/// Deselects a tile.
	void deselectTile();
	/// Clicks at a tile.
	/// 
	/// @param pos Tile position.
	void click(sf::Vector2i pos);

	/// Constructs a region UI panel.
	/// 
	/// @param region Region object.
	/// @param targeted Whether the menu is targeted.
	void regionMenu(const Region& region, bool targeted = false);

	/// Constructs a troop UI panel.
	/// 
	/// @param troop Selected troop.
	void troopMenu(const Troop& troop);
	/// Constructs a building UI panel.
	/// 
	/// @param build Selected building.
	void buildMenu(const Build& build);
	/// Constructs a plant UI panel.
	/// 
	/// @param plant Selected plant.
	void plantMenu(const Plant& plant);
	/// Constructs a hex UI panel.
	/// 
	/// @param hex Selected hex.
	void hexMenu(const Hex& hex);

	/// Closes any open menus.
	void closeMenu();

	/// Returns hex coordinates at a mouse position.
	///
	/// @param mouse Mouse position.
	sf::Vector2i mouseToHex(sf::Vector2i mouse) const;

protected:
	/// Draws the map.
	void drawSelf(ui::RenderBuffer& target, sf::IntRect self) const override;
};