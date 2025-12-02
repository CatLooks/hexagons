#include "game/ui/action_button.hpp"

namespace gameui {
	/// Action button side length.
	const ui::Dim Action::side = (float)(64 * Values::k);
	/// Action button base size.
	const ui::DimVector Action::base = { side, side };

	/// Action button texture maps.
	const ui::Panel::Map Action::textures[2] = {
		ui::Panel::Map::rect(&assets::interface, Values::coords(0, 0), { 6, 6 }, 2),
		ui::Panel::Map::rect(&assets::interface, Values::coords(1, 0), { 6, 6 }, 2),
	};

	/// Constructs an empty action button.
	Action::Action(int hint) : ui::Panel(textures[0]), _hint(hint) {
		// set action button size
		bounds.size = base;

		// reset padding
		padding.set(0);

		// add hint
		if (hint >= 0 && hint <= 9) {
			ui::Image* himg = new ui::Image(&assets::interface, Values::digits[hint]);
			himg->bounds = {
				{ 0as + Values::buttonBorder, 0as + Values::buttonBorder },
				Values::iconSize
			};
			add(himg);
		};

		// event handler
		onEvent([=](const ui::Event& evt) {
			// check for mouse click
			if (auto data = evt.get<ui::Event::MousePress>()) {
				// left mouse button
				if (data->button == sf::Mouse::Button::Left) {
					click();
					return true;
				};
			};

			// absorb all mouse events
			return (bool)evt.mouse();
		});
	};

	/// Clears the action button.
	void Action::clear() {
		// remove elements
		ui::Panel::clear();
		ui::Panel::clears();

		// reset pointers
		_tex = nullptr;
		_ann = nullptr;
		_text = nullptr;
		_sub = nullptr;
		_draw = nullptr;
	};

	/// Adds an annotation icon to the action button.
	void Action::annotate(SkillDesc::Annotation ann) {
		if (!_ann) {
			// ignore if no annotation
			if (ann == SkillDesc::None) return;

			// create annotation icon
			_ann = new ui::Image(&assets::interface, Values::annotations[static_cast<int>(ann)]);
			_ann->bounds = {
				{ 1as - Values::buttonBorder, 1as - Values::buttonBorder },
				Values::iconSize
			};
			add(_ann);
		} else {
			// change coordinates
			_ann->coords = Values::annotations[static_cast<int>(ann)];
		};
	};

	/// Adds an image to the action button.
	void Action::setTexture(const sf::Texture* texture, sf::IntRect map) {
		if (!_tex) {
			_tex = new ui::Image(texture, map);
			_tex->position() = { 0.5as, 0.5as };
			_tex->size() *= (float)Values::k;
			adds(_tex);
		}
		else {
			_tex->texture = texture;
			_tex->coords = map;
		};
	};

	/// Adds an extra draw call to the action button.
	void Action::setDraw(DrawCall call) {
		_draw = call;
	};

	/// Adds a callback to pressed action button.
	void Action::setCall(Callback press, Callback release, Mode mode) {
		_press = press;
		_release = release;
		_mode = mode;
	};

	/// Expands the button.
	ui::Anim* Action::emitExpand() {
		return new ui::AnimVector(&size(), base, base * 1.125f, sf::seconds(0.06f));
	};
	/// Shrinks the button.
	ui::Anim* Action::emitShrink() {
		return ui::AnimVector::to(&size(), base, sf::seconds(0.1f));
	};

	/// Forcefully invokes the action callback.
	void Action::click() {
		switch (_mode) {
		case Click:
			// invoke callbacks
			if (_press) _press();
			if (_release) _release();

			// button pulse animation
			push(chain(emitExpand(), emitShrink()));
			break;
		default:
			if (!_state) {
				// press button
				if (_press) _press();
				_state = true;

				// button animation
				_map = textures[1];
				push(emitExpand());
			}
			else {
				// release button
				if (_release) _release();
				_state = false;

				// button animation
				deselect();
			};
			break;
		};
	};
	/// Deselects the button.
	void Action::deselect() {
		if (_state) {
			_map = textures[0];
			push(emitShrink());
		};
	};

	/// Adds text to the action button.
	ui::Text* Action::setLabel() {
		if (!_text) {
			// create empty label
			_text = new ui::Text(Values::panel_text, {});
			_text->bounds = { 0px, -24px, 1ps, 0px };
			_text->align = ui::Text::N;
			_text->pos = ui::Text::Static;

			// register text element
			add(_text);
		};
		return _text;
	};

	/// Adds text to the action button.
	ui::Text* Action::setSubtitle() {
		if (!_sub) {
			// create empty label
			_sub = new ui::Text(Values::panel_text, {});
			_sub->bounds = { 0px, -44px, 1ps, 0px };
			_sub->align = ui::Text::N;
			_sub->pos = ui::Text::Static;

			// register text element
			add(_sub);
		};
		return _sub;
	};

	/// Draws the action button.
	void Action::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
		ui::Panel::drawSelf(target, self);
		if (_draw) _draw(target, self);
	};
};