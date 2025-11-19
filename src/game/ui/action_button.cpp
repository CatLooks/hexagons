#include "game/ui/action_button.hpp"

namespace gameui {
	/// Action button size.
	const ui::Dim Action::size = (float)(64 * Values::k);

	/// Action button texture maps.
	const ui::Panel::Map Action::textures[2] = {
		ui::Panel::Map::rect(&assets::interface, Values::coords(0, 0), { 6, 6 }, 2),
		ui::Panel::Map::rect(&assets::interface, Values::coords(1, 0), { 6, 6 }, 2),
	};

	/// Constructs an empty action button.
	Action::Action(int hint) : ui::Panel(textures[0]), _hint(hint) {
		// set action button size
		bounds.size = { size, size };

		// reset padding
		padding.set(0);

		// add hint
		if (hint >= 0 && hint <= 9) {
			ui::Image* himg = new ui::Image(&assets::interface, Values::digits[hint]);
			himg->bounds = { { 1es / 8, 1es / 8 }, Values::iconSize };
			add(himg);
		};
	};

	/// Adds an image to the action button.
	void Action::addTexture(sf::IntRect map) {
		ui::Image* dimg = new ui::Image(&assets::interface, map);
		dimg->bounds = ui::DimRect::Fill;
		adds(dimg);
	};

	/// Adds a text to the action button.
	ui::Text* Action::addLabel() {
		// create empty label
		_text = new ui::Text(Values::panel_text, {});
		_text->bounds = { 0px, -24px, 1ps, 0px };
		_text->align = ui::Text::N;
		_text->pos = ui::Text::Static;

		// register text element
		add(_text);
		return _text;
	};
};