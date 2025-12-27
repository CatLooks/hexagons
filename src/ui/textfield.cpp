#include "ui/textfield.hpp"

namespace ui {
	/// Constructs a text field element.
	TextField::TextField(const Panel::Map& map, const TextSettings& settings, sf::Color cursor):
		Panel(map),
		_label(ui::Text::raw(settings, "")),
		_cursor(new Solid),
		_focused(false)
	{
		scissor = true;  // hide text overflow
		infinite = true; // capture events outside of draw area

		// configure cursor
		_cursor->bounds = { 0px, 0.5as, 3px, 1ps - 4px };
		_cursor->color = cursor;
		{
			// add cursor animation
			auto* anim = new ui::AnimSet(sf::seconds(0.5f), [=](float t) {
				_cursor->color.a = _focused ? (uint8_t)t : 0;
			});
			anim->setRange(0.f, 255.f);
			anim->ease = ui::Easings::sineOut;
			anim->mode = ui::Anim::Bounce;
			_cursor->push(anim);
		};

		// configure text
		_label->align = ui::Text::W;
		_label->pos = ui::Text::Static;

		// add child elements
		adds(_label);
		adds(_cursor);

		// configure input manager
		input.attachTextDisplay([=](const sf::String& string) {
			// update displayed text
			_label->setRaw(string);
		});
		input.attachCursorFocus([=](unsigned int position) {
			// update cursor position
			_cursor->position().x = _label->position().x + _label->charpos(position).x;
		});

		// attach event pipes
		onEvent([=](const ui::Event& evt) {
			// mouse focus / unfocus checks
			if (auto data = evt.get<ui::Event::MousePress>()) {
				focus(rect().contains(data->position));
				return _focused;
			};

			// ignore if not focused
			if (!_focused) return false;

			// pass through keyboard events
			if (auto data = evt.get<ui::Event::KeyPress>())
				return input.eventKey(*data);
			if (auto data = evt.get<ui::Event::CharEnter>())
				return input.eventChar(*data);

			// pass through
			return false;
		});
	};

	/// Updates text field focus.
	void TextField::focus(bool enabled) {
		_focused = enabled;
		ui::window.setKeyRepeat(enabled);
	};

	/// Automatically unfocuses field on deactivation.
	void TextField::onDeactivate() {
		focus(false);
	};
};