#include "game/ui/state_viewer.hpp"
#include "game/values/hex_values.hpp"

namespace gameui {
	/// Bar height.
	const ui::Dim State::height = 56px;

	/// Constructs a game state viewer element.
	State::State(GameState* state) : _state(state) {
		// configure bar panel
		bounds = { 0px, 0px, 1ps, height };
		color = Values::dimTint;

		// element width
		static const ui::Dim width = 1ps / 3;

		// offset from top
		static const ui::Dim offset = 8px;

		// turn number text
		_turn = new ui::Text(Values::state_text, "gp.turn");
		_turn->align = ui::Text::NW;
		_turn->bounds = { 16px, offset, width, 1ps - offset };
		add(_turn);

		// middle part element
		_mid = new ui::Align(16);
		_mid->bounds = { 1ts, 0px, width, 1ps };
		add(_mid);

		// current player info labels
		_team = new ui::Text(Values::state_text, "gp.team");
		_now = new ui::Text(Values::state_text, "gp.player");
		_time = new ui::Text(Values::state_text, "param");

		// configure labels
		ui::Text* labels[3] = { _team, _now, _time };
		for (size_t i = 0; i < 3; i++) {
			// configure & register label
			labels[i]->autosize = true;
			labels[i]->pos = ui::Text::Static;
			labels[i]->position().y = offset;
			_mid->add(labels[i]);

			// add bar separator
			if (i != 2) {
				auto* bar = new ui::Solid;
				bar->size().x = 2px;
				bar->margin.setVertical(6);
				_mid->add(bar);
			};
		};

		// next player info text
		_next = new ui::Text(Values::state_text, "gp.next");
		_next->align = ui::Text::NE;
		_next->bounds = { 1as - 16px, offset, width, 1ps - offset };
		add(_next);

		// attach label hooks
		onRecalculate([=](const sf::Time& delta) {
			// update turn number
			_turn->param("turn", ext::str_int(state->turn()));

			// update current player info
			const auto* player = state->player();
			_now->param("name", player->name);
			_team->param("team", Values::hex_names[player->team]);
			_team->setColor(Values::hex_colors[player->team]);

			// update turn timer
			_time->param("value", ext::str_time(state->turnTime().asSeconds()));

			// update next player info
			const auto* next = state->nextPlayer();
			_next->param("team", Values::hex_names[next->team]);
			_next->setColor(Values::hex_colors[next->team]);
		});
	};
};