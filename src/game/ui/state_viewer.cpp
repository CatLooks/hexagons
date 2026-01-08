#include "game/ui/state_viewer.hpp"

namespace gameui {
	/// Bar height.
	const ui::Dim State::height = 48px;

	/// Constructs a game state viewer element.
	State::State(GameState* state) : _state(state) {
		// configure bar panel
		bounds = { 0px, 0px, 1ps, height };
		color = Values::dimTint;
	};
};