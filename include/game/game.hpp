#pragma once

// include dependencies
#include "ui/layer.hpp"
#include "ui/camera.hpp"
#include "ui/window.hpp"
#include "map.hpp"
#include "game/ui/game_panel.hpp"

/// Game controller object.
class Game : public ui::Element {
public:
	Map map; /// Game map.

private:
	ui::Layer*  _layer; /// Render layer.
	gameui::Panel* _panel;
	ui::Camera _camera; /// Map camera.

public:
	/// Constructs a game object.
	///
	/// @param layer Render layer.
	Game(ui::Layer* layer, gameui::Panel* panel);

	/// Returns hex coordinates at a mouse position.
	///
	/// @param mouse Mouse position.
	sf::Vector2i mouseToHex(sf::Vector2i mouse) const;

protected:
	/// Draws the map.
	void drawSelf(ui::RenderBuffer& target, sf::IntRect self) const override;
};