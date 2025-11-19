#include "game/ui/game_panel.hpp"
#include "assets.hpp"

namespace gameui {
	/// Game control panel map.
	const ui::Panel::Map Panel::texture =
		ui::Panel::Map::rect(&assets::interface, Values::coords(2, 0), { 4, 4 }, 2);

	/// Panel height.
	const ui::Dim Panel::height = float(48 * Values::k);
	/// Action box spacing.
	const ui::Dim Panel::spacing = Action::size * 1.6f;

	/// Box spacing table.
	static const std::vector<float> spacing_table[Panel::Count] = {
		/* --- */ {},
		/* L00 */ {},
		/* L10 */ { -1.5f },
		/* L01 */ {  1.5f },
		/* L11 */ { -1.5f,  1.5f },
		/* L21 */ { -2.0f, -1.0f, 1.5f },
		/* L12 */ { -1.5f,  1.0f, 2.0f },
		/* L22 */ { -2.0f, -1.0f, 1.0f, 2.0f }
	};

	/// Amount of actions in layout.
	static const int box_count[Panel::Count] = {
		0, 0, 1, 1, 2, 3, 3, 4
	};

	/// Constructs the game panel.
	Panel::Panel(): ui::Panel(texture), _layout(L00) {
		// set panel bounds
		bounds = { 0px, 1as, 1ps, height };
		event_scissor = false;
		padding.set(0);

		// create preview box
		_preview = new Action();
		{
			_preview->position() = { 0.5as, height - Action::size };
			_preview->deactivate();
		};
		adds(_preview);
	};

	/// Reconstructs panel actions.
	void Panel::construct(Layout layout) {
		// delete previous boxes
		_boxes.clear();
		clear();

		// construct new boxes
		int count = box_count[layout];
		for (int i = 0; i < count; i++) {
			// create action button
			Action* action = new Action(i + 1);
			action->position() = {
				0.5as + spacing * spacing_table[layout].at(i),
				height - Action::size
			};

			// register action button
			_boxes.push_back(action);
			add(action);
		};

		// reset preview
		_preview->clear();
		if (layout)
			_preview->activate();
		else
			_preview->deactivate();

		// recalculate panel
		_layout = layout;
		recalculate();
	};

	/// Returns a reference to action panels.
	const std::vector<Action*>& Panel::actions() const {
		return _boxes;
	};

	/// Returns a preview panel.
	Action* Panel::preview() const {
		return _preview;
	};
};