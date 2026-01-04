#include "game/ui/game_panel.hpp"
#include "assets.hpp"

namespace gameui {
	/// Game control panel map.
	const ui::Panel::Map Panel::texture =
		ui::Panel::Map::rect(&assets::interface, Values::coords(2, 0), { 4, 4 }, 2);

	/// Panel height.
	const ui::Dim Panel::height = float(48 * Values::k);
	/// Action box spacing.
	const ui::Dim Panel::spacing = Action::side * 1.6f;

	/// Box spacing table.
	static const std::vector<float> spacing_table[logic::SkillArray::Count] = {
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
	static const int box_count[logic::SkillArray::Count] = {
		0, 0, 1, 1, 2, 3, 3, 4
	};

	/// Constructs the game panel.
	Panel::Panel(const History& history): ui::Panel(texture), _layout(logic::SkillArray::None) {
		// set panel bounds
		bounds = { 0px, 1ps + Action::side, 1ps, height };
		event_scissor = false;
		padding.set(0);

		// create preview box
		_preview = new Action();
		{
			_preview->position() = { 0.5as, 1as };
			_preview->bounds.size *= 1.2f;
			_preview->display = true;
			_preview->deactivate();
		};
		adds(_preview);

		// create move selector
		_select = new Selector();
		_select->position().x = 1as + Selector::width * 2;
		adds(_select);

		// add move selector update
		onRecalculate([this, &history](const sf::Time&) {
			_select->update(history.count());
		});

		// absorb all events
		onEvent([=](const ui::Event& evt) {
			if (auto data = evt.get<ui::Event::KeyPress>()) {
				// button key map
				static const sf::Keyboard::Key mapping[4] = {
					sf::Keyboard::Key::Num1, sf::Keyboard::Key::Num2,
					sf::Keyboard::Key::Num3, sf::Keyboard::Key::Num4
				};

				// check all buttons
				for (size_t i = 0; i < _boxes.size(); i++) {
					if (data->key == mapping[i])
						_boxes[i]->click();
				};
			};

			// absorb all mouse events
			return (bool)evt.mouse();
		});
	};

	/// Reconstructs panel actions.
	void Panel::construct(logic::SkillArray::Layout layout) {
		// show / hide animations
		if ((_layout == logic::SkillArray::None) != (layout == logic::SkillArray::None)) {
			if (layout != logic::SkillArray::None) {
				// show panel
				ui::Anim* anim0 = ui::AnimDim::to(&position().y, 1as, sf::seconds(0.2f));
				anim0->ease = ui::Easings::quadOut;
				push(anim0);

				// show selectors
				ui::Anim* anim1 = ui::AnimDim::to(&_select->position().x, 1as, sf::seconds(0.2f));
				anim1->ease = ui::Easings::quadOut;
				push(anim1);
			}
			else {
				// hide panel
				ui::Anim* anim0 = ui::AnimDim::to(&position().y, 1ps + Action::side, sf::seconds(0.2f));
				anim0->ease = ui::Easings::quadIn;
				push(anim0);

				// hide selectors
				ui::Anim* anim1 = ui::AnimDim::to(&_select->position().x, 1as + Selector::width * 2, sf::seconds(0.2f));
				anim1->ease = ui::Easings::quadOut;
				push(anim1);
			};
		};

		// delete previous boxes
		_boxes.clear();
		clear();

		// construct new boxes
		int count = box_count[layout];
		for (int i = 0; i < count; i++) {
			// create action button
			Action* action = new Action(i + 1);
			action->position() = { 0.5as + spacing * spacing_table[layout].at(i), 1as };

			// register action button
			_boxes.push_back(action);
			add(action);
		};

		// update preview visibility
		_preview->clear();
		if (layout != logic::SkillArray::None)
			_preview->activate();
		else
			_preview->deactivate();

		// recalculate panel
		_layout = layout;
		recalculate();
	};

	/// Updates action timers.
	void Panel::timers(uint8_t* timers) {
		size_t i = 0;
		for (Action* action : _boxes)
			action->setTimer(timers[i++]);
	};

	/// Returns preview box.
	Action* Panel::preview() const {
		return _preview;
	};

	/// Returns a reference to action panels.
	const std::vector<Action*>& Panel::actions() const {
		return _boxes;
	};

	/// Returns move selector container element.
	Selector* Panel::selector() const {
		return _select;
	};
};