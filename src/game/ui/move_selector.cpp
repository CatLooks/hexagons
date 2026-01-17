#include "game/ui/move_selector.hpp"

namespace gameui {
	/// Move selector button texture map.
	const ui::Panel::Map Selector::texture = ui::Panel::Map::rect
		(&assets::interface, Values::coords(0, 0), { 6, 6 }, 2);

	/// Selector button width.
	const ui::Dim Selector::width = 80px;

	/// Constructs a move selector element.
	Selector::Selector() {
		event_scissor = false;

		// create turn button
		_next = new ui::Button(texture, { width, width });
		_next->setIcon(&assets::interface, Values::next_icon);
		{
			_next_t = _next->setLabel(Values::panel_text);
			_next_t->pos = ui::Text::ShrinkY;
			_next_t->position() = { -4px - 1ts, 0.5as };
			_next_t->autosize = true;
		};
		add(_next);

		// create redo button
		_redo = new ui::Button(texture, { width, width });
		_redo->setIcon(&assets::interface, Values::redo_icon);
		{
			_redo_t = _redo->setLabel(Values::panel_text);
			_redo_t->pos = ui::Text::ShrinkY;
			_redo_t->position() = { -4px - 1ts, 0.5as };
			_redo_t->autosize = true;
			_redo_t->setColor(sf::Color::Red);
			_redo_t->setPath("param");
		};
		_redo->disable();
		add(_redo);

		// create undo button
		_undo = new ui::Button(texture, { width, width });
		_undo->setIcon(&assets::interface, Values::undo_icon);
		{
			_undo_t = _undo->setLabel(Values::panel_text);
			_undo_t->pos = ui::Text::ShrinkY;
			_undo_t->position() = { -4px - 1ts, 0.5as };
			_undo_t->autosize = true;
			_undo_t->setColor(sf::Color::Green);
			_undo_t->setPath("param");
		};
		_undo->disable();
		add(_undo);

		// setup button layout
		auto y = 0.5as + 0.5ps - width * 0.6f;
		for (const auto& button : children()) {
			// set button position
			button->position() = { 1as, y };
			y -= width * 1.125f; // 1 + gap between

			// absorb all mouse events
			button->onEvent([](const ui::Event& evt) {
				return (bool)evt.mouse();
			});
		};

		// add turn label animation
		_next_t->onUpdate([=](const sf::Time&) {
			// label variants
			static const char* states[] = {
				".", "..", "..."
			};

			// update text label
			_next_t->setRaw(states[_prog % 3]);
			_next_t->recalculate();
		});
		_next_t->deactivate();
	};

	/// Attaches callbacks to move selector element.
	void Selector::attach(StaticHandler undo, StaticHandler redo, std::function<bool()> next) {
		// attach callbacks
		_undo->attach(undo);
		_redo->attach(redo);

		// attach validation
		_undo->validate([=]() { return _enabled; });
		_redo->validate([=]() { return _enabled; });
		_next->validate(next);
	};

	/// Updates move counter.
	void Selector::update(const std::pair<size_t, size_t>& moves) const {
		// update button labels
		_undo_t->param("value", std::format("{}", moves.first));
		_redo_t->param("value", std::format("{}", moves.second));
		
		// enable / disable buttons
		if (moves.first) _undo->enable(); else _undo->disable();
		if (moves.second) _redo->enable(); else _redo->disable();
	};

	/// Enables or disables the move selectors.
	void Selector::enable(bool enabled) {
		_enabled = enabled;
		if (enabled) {
			// disable pending animation
			if (_pending) {
				_pending->cancel();
				_pending = nullptr;
			};
			_next_t->deactivate();
		}
		else {
			// create pending animation
			if (!_pending) {
				_pending = new ui::AnimInt(&_prog, 0, 3, sf::seconds(1.f));
				_pending->mode = ui::Anim::Loop;
				_next_t->push(_pending);
				_prog = 0;
			}
			else _pending->restart();
			_next_t->activate();
		};
	};
};