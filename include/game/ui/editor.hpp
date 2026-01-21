#pragma once

// include dependencies
#include "game/values/interface.hpp"
#include "game/values/editor_values.hpp"
#include "game/map.hpp"
#include "game/moves/move.hpp"

#include "resource_table.hpp"
#include "map_loader.hpp"

#include "assets.hpp"
#include "ui.hpp"

class Game;

/// Editor elements container.
class Editor : public ui::Element {
public:
	/// Editor button side length.
	static const ui::Dim side;

	/// Current tile team.
	Region::Team team = Region::Unclaimed;

	/// Entity clipboard.
	Moves::EntState entity = {};

private:
	Game* _game; /// Game controller reference.

	gameui::Table* _res_table; /// Resource table.
	gameui::Loader* _loader;   /// Loader menu.

	ui::Text* _team_text; /// Tile team text.
	ui::Text* _show_text; /// Selected entity text.
	ui::Text* _clip_text; /// Entity clipboard text.

	ui::Button* _grab; /// Map loader grabber.

	/// Update text labels.
	void updateLabels();

	/// Updates entity label.
	/// 
	/// @param text Label element.
	/// @param entity Entity state.
	static void updateEntity(ui::Text* text, const Moves::EntState& entity);

	/// Returns selected tile.
	HexRef tileref() const;

public:
	/// Construct editor environment.
	///
	/// @param game Game controller reference.
	/// @param text_layer Text field layer.
	Editor(Game* game, ui::Layer* text_layer);

	/// Selects a region.
	/// 
	/// @param reg Region reference.
	void region(Region* reg);

	/// Hides the loader grabber button.
	void hide();

	/// Whether any input field is active.
	bool input() const;
};