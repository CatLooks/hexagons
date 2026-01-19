#pragma once

// include dependencies
#include "game/values/interface.hpp"
#include "game/values/editor_values.hpp"
#include "game/map.hpp"
#include "game/moves/move.hpp"
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

	ui::Text* _team_text; /// Tile team text.
	ui::Text* _show_text; /// Selected entity text.
	ui::Text* _clip_text; /// Entity clipboard text.

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
	Editor(Game* game);
};