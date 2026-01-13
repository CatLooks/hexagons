#include "game/ui/action_button.hpp"
#include "mathext.hpp"

namespace gameui {
	/// Action button side length.
	const ui::Dim Action::side = (float)(64 * Values::k);
	/// Action button base size.
	const ui::DimVector Action::base = { side, side };

	/// Maximum opacity color of red tint during error animation.
	const sf::Color Action::red = sf::Color(235, 0, 0, 160);

	/// Minimum opacity color of red tint during error animation.
	static const sf::Color red_dimmed = []() {
		sf::Color color = Action::red;
		color.a = 0;
		return color;
	}();

	/// Overlay color for action on cooldown.
	const sf::Color Action::dim = sf::Color(0, 0, 0, 128);

	/// Action button texture maps.
	const ui::Panel::Map Action::textures[2] = {
		ui::Panel::Map::rect(&assets::interface, Values::coords(0, 0), { 6, 6 }, 2),
		ui::Panel::Map::rect(&assets::interface, Values::coords(1, 0), { 6, 6 }, 2),
	};

	/// Constructs an empty action button.
	Action::Action(int hint) : ui::Panel(textures[0]), _hint(hint), _timer(0) {
		// set action button size
		bounds.size = base;

		// create button overlay
		clear();

		// reset padding
		padding.set(0);

		// add hint
		if (hint >= 0) {
			// cap at '?'
			if (hint > 10) hint = 10;

			// generate hint icon
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
		_cdi = nullptr;
		_tex = nullptr;
		_ann = nullptr;
		_text = nullptr;
		_sub = nullptr;
		_draw = nullptr;

		// create error overlay
		_err = new ui::Solid;
		_err->color = red_dimmed;
		add(_err);
	};

	/// Adds an annotation icon to the action button.
	void Action::annotate(Skill::Annotation ann) {
		if (!_ann) {
			// ignore if no annotation
			if (ann == Skill::None) return;

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

	/// Adds a timer icon to the action button.
	void Action::setTimer(uint8_t timer) {
		// store timer
		_timer = timer;

		// cap displayed value
		if (timer > 10) timer = 10;

		// create timer icon
		if (!_cdi) {
			if (!timer) return; // ignore if timer is 0

			_cdi = new ui::Image(&assets::interface, Values::digits[timer]);
			_cdi->bounds = {
				{ 0as + Values::buttonBorder, 1as - Values::buttonBorder },
				Values::iconSize
			};
			_cdi->tint = Values::cooldown_color;
			add(_cdi);
		};

		// change icon state
		if (timer) {
			// enable timer
			_cdi->coords = Values::digits[timer];
			_cdi->activate();
			_err->color = dim;
		}
		else {
			// disable timer
			_cdi->deactivate();
			_err->color = red_dimmed;
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

	/// Adds a button click validation.
	void Action::setCheck(Validation call) {
		_check = call;
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

	/// Shaker function interpolation.
	///
	/// @param t Interpolation parameter.
	static float shake(float t) {
		const int shake_count = 2;
		// 1 shake = mid -> right -> mid -> left -> mid
		return sinf(shake_count * 2 * (float)M_PI * t);
	};

	/// Shakes the button and highlights it in red.
	void Action::errorShake() {
		// ignore if already shaking
		if (_shake) return;

		// shake displacement
		const ui::DimVector shake_amp = { 1me / 16.f, 0px };

		// store original position
		ui::DimVector pos = position();
		_shake = true;

		// create shake animation
		{
			ui::Anim* anim = ui::AnimVector::to(&position(), pos + shake_amp, sf::seconds(0.4f));
			anim->setAfter([=]() { _shake = false; });
			anim->ease = shake;
			push(anim);
		};

		// create red tint animation
		{
			ui::Anim* anim_in = ui::AnimColor::to(&_err->color, red, sf::seconds(0.05f));
			ui::Anim* anim_out = new ui::AnimColor(&_err->color, red, _timer > 0 ? dim : red_dimmed, sf::seconds(0.3f));
			anim_out->ease = ui::Easings::quadIn;
			anim_out->setAfter([=]() {
				// set color again
				// in case timer has changed in the middle of the animation
				push(ui::AnimColor::to(&_err->color, _timer > 0 ? dim : red_dimmed, sf::seconds(0.05f)));
			});
			push(chain(anim_in, anim_out));
		};
	};

	/// Forcefully invokes the action callback.
	void Action::click() {
		// check if validation fails
		if (!_state && ((_check && !_check()) || _timer)) {
			// button shake animation
			errorShake();
			return;
		};

		// handle button click
		switch (_mode) {
		case Click:
			_state = false;

			// invoke callbacks
			if (_press) _press();
			if (_release) _release();

			// button pulse animation
			if (!display)
				push(chain(emitExpand(), emitShrink()));
			break;
		default:
			if (!_state) {
				// press button
				if (_press) _press();
				_state = true;

				// button animation
				_map = textures[1];
				if (!display) push(emitExpand());
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
			_state = false;
			_map = textures[0];
			if (!display) push(emitShrink());
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

	/// Makes the overlay the topmost element.
	void Action::forwardOverlay() {
		to_front(_err);
	};
};