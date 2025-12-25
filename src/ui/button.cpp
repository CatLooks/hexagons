#include "ui/button.hpp"
#include "ui/window.hpp"

namespace ui {
	/// Emits a button expansion animation.
	Anim* Button::emitExpand(const DimVector& size) {
		return AnimVector::to(&this->size(), size, sf::seconds(0.06f));
	};

	/// Emits a button expansion animation.
	Anim* Button::emitShrink(const DimVector& size) {
		return AnimVector::to(&this->size(), size, sf::seconds(0.1f));
	};

	/// Shaker function interpolation.
	///
	/// @param t Interpolation parameter.
	static float shake(float t) {
		const int shake_count = 2;
		// 1 shake = mid -> right -> mid -> left -> mid
		return sinf(shake_count * 2 * (float)M_PI * t);
	};

	/// Starts an error animation.
	void Button::startError() {
		// ignore if already shaking
		if (_shake) return;

		// shake displacement
		const ui::DimVector shake_amp = { 1es / 16, 0px };

		// store original position
		ui::DimVector pos = position();
		_shake = true;

		// create shake animation
		{
			_anim = ui::AnimVector::to(&position(), pos + shake_amp, sf::seconds(0.4f));
			_anim->setAfter([=]() { _shake = false; });
			_anim->ease = shake;
			push(_anim);
		};

		// create red tint animation
		{
			sf::Color tp = red;
			tp.a = 0;
			ui::Anim* anim_in = ui::AnimColor::to(&_overlay->color, red, sf::seconds(0.05f));
			ui::Anim* anim_out = new ui::AnimColor(&_overlay->color, red, tp, sf::seconds(0.3f));
			anim_out->ease = ui::Easings::quadIn;
			push(chain(anim_in, anim_out));
		};
	};

	/// Constructs button element base.
	Button::Button(const Panel::Map& map, const DimVector& size)
		: Panel(map), _size(size)
	{
		bounds.size = size;
		padding.set(0);

		// create button overlay
		clear();

		// add mouse click event
		onEvent([=](const Event& evt) {
			if (auto data = evt.get<Event::MousePress>()) {
				if (data->button == sf::Mouse::Button::Left) {
					// mouse press
					force();
					return false;
				};
			};

			// ignore event
			return false;
		});
	};

	/// Clears button contents.
	void Button::clear() {
		// reset child elements
		Element::clear();
		_text = nullptr;
		_image = nullptr;

		// create button overlay
		_overlay = new Solid;
		_overlay->color = sf::Color::Transparent;
		add(_overlay);
	};

	/// Creates a button label element.
	ui::Text* Button::setLabel(const TextSettings& settings) {
		if (!_text) {
			_text = new ui::Text(settings, {});
			_text->align = ui::Text::Center;
			_text->setPositioning(ui::Text::Static | ui::Text::ShrinkX);
			add(_text);
		};
		return _text;
	};

	/// Creates a button icon element.
	ui::Image* Button::setIcon(const sf::Texture* texture, sf::IntRect coords) {
		if (!_image) {
			_image = new ui::Image(texture, coords);
			_image->position() = { 0.5as, 0.5as };
			add(_image);
		};
		return _image;
	};

	/// Forwards the overlay element to the top.
	void Button::forwardOverlay() {
		to_front(_overlay);
	};

	/// Binds a keyboard key to the button.
	void Button::bindKey(sf::Keyboard::Key key) {
		onEvent([=](const Event& evt) {
			if (auto data = evt.get<Event::KeyPress>()) {
				if (data->key == key) {
					force();
					return true;
				};
			};

			// ignore event
			return false;
		});
	};

	/// Adds a callback to the button.
	void Button::attach(std::function<void()> call) {
		_call.add(call);
	};

	/// Sets a click validation callback.
	void Button::validate(std::function<bool()> check) {
		_check = check;
	};

	/// Disables the button.
	void Button::disable() {
		_disabled = true;
		if (_anim && _anim->active()) _anim->end();
		_overlay->color = dim;
	};

	/// Enables the button.
	void Button::enable() {
		_disabled = false;
		_overlay->color.a = 0;
	};

	/// Forces a button click event.
	void Button::force() {
		// ignore if disabled
		if (_disabled) return;
		if (_check()) {
			// invoke callback delegate
			_call.invoke();

			// create button pulse animation
			push(chain(emitExpand(_size * 1.125f), emitShrink(_size)));
		}
		else {
			// create error animation
			startError();
		};
	};
};