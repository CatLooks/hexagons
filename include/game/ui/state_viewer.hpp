#pragma once

// include dependencies
#include "game/values/interface.hpp"
#include "game/sync/state.hpp"
#include "ui.hpp"

namespace gameui {
	/// Game state viewer.
	class State : public ui::Solid {
	public:
		/// Bar height.
		static const ui::Dim height;

	private:
		GameState* _state; /// State reference.

	public:
		/// Constructs a game state viewer element.
		/// 
		/// @param state Game state reference.
		State(GameState* state);
	};
};