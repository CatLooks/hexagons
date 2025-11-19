#include "game/ui/game_panel.hpp"
#include "assets.hpp"

namespace gameui {
	/// Game control panel map.
	const ui::Panel::Map Panel::textures[3] = {
		ui::Panel::Map::rect(&assets::interface, Values::coords(2, 0), { 4, 4 }, 2),
		ui::Panel::Map::rect(&assets::interface, Values::coords(0, 0), { 6, 6 }, 2),
		ui::Panel::Map::rect(&assets::interface, Values::coords(1, 0), { 6, 6 }, 2),
	};

	/// Panel height.
	const ui::Dim Panel::height = ui::Dim(48 * Values::k);
	/// Action box size.
	const ui::Dim Panel::box = ui::Dim(64 * Values::k);
	/// Action box spacing.
	const ui::Dim Panel::spacing = Panel::box * 1.5;

	/// Box spacing table.
	const std::vector<float> Panel::spacing_table[Panel::Count] = {
		/* L00 */ {},
		/* L10 */ { -1.5f },
		/* L01 */ {  1.5f },
		/* L11 */ { -1.5f,  1.5f },
		/* L21 */ { -2.0f, -1.0f, 1.5f },
		/* L12 */ { -1.5f,  1.0f, 2.0f },
		/* L22 */ { -2.0f, -1.0f, 1.0f, 2.0f }
	};

	/// Constructs the game panel.
	Panel::Panel(): ui::Panel(textures[Lower]) {
		// set panel bounds
		bounds = { 0px, 1as, 1ps, height };
		event_scissor = false;
		padding.set(0);

		// create preview box
		_preview = new ui::Panel(textures[Normal]);
		{
			_preview->bounds = { 0.5as, height - box, box, box };
		};
		add(_preview); // @todo: changed to adds()
	};

	/// Reconstructs panel actions.
	void Panel::construct(Layout layout) {
		// delete previous boxes
		_boxes.clear();
		clear();

		// construct new boxes
		int count = (layout >> 4) + (layout & 15);
		for (int i = 0; i < count; i++) {
			ui::Panel* action = new ui::Panel(textures[Normal]);
			action->bounds = {
				0.5as + spacing * spacing_table[layout].at(i),
				height - box, box, box
			};
			_boxes.push_back(action);
			add(action);
		};

		// create preview box
		_preview = new ui::Panel(textures[Normal]);
		{
			_preview->bounds = { 0.5as, height - box, box, box };
		};
		add(_preview); // @todo: remove this
	};

	/// Returns a reference to action panels.
	const std::vector<ui::Panel*>& Panel::actions() const {
		return _boxes;
	};
};