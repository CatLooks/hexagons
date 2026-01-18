#pragma once

// include dependencies
#include "game/values/interface.hpp"
#include "game/sync/state.hpp"
#include "mathext.hpp"
#include "ui.hpp"

namespace gameui {
	/// Game state viewer.
	class State : public ui::Solid {
	public:
		/// Bar height.
		static const ui::Dim height;

	private:
		GameState* _state; /// State reference.

		ui::Text* _turn; /// Turn number.
		ui::Text*  _now; /// Current player.
		ui::Text* _team; /// Current team.
		ui::Text* _next; /// Next player.
		ui::Text* _time; /// Current move timer.
		ui::Align* _mid; /// Middle section alignment element.

	public:
		/// Constructs a game state viewer element.
		/// 
		/// @param state Game state reference.
		State(GameState* state);
	};
};