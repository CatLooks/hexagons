#pragma once

// include dependencies
#include "panel.hpp"
#include "image.hpp"
#include "text.hpp"
#include "anim/linear.hpp"
#include "anim/setter.hpp"
#include "mathext.hpp"
#include <delegate>

namespace ui {
	/// Generic button element.
	class Button : public Panel {
	public:
		/// Disabled button overlay color.
		sf::Color dim = sf::Color(0, 0, 0, 128);
		/// Errored button overlay color.
		sf::Color red = sf::Color(255, 32, 64);
		/// Button expansion coefficient.
		float exp_coef = 1.125f;

	protected:
		Solid* _overlay {}; /// Button overlay.
		Anim*     _anim {}; /// Overlay animation.
		Text*     _text {}; /// Button text.
		Image*   _image {}; /// Button image.
		bool  _disabled {}; /// Whether the button is disabled.
		bool     _shake {}; /// Whether the button is shaking.
		DimVector _size {}; /// Base button size.

		/// Button click check.
		std::function<bool()> _check = []() { return true; };

		/// Button callback delegate.
		Delegate<void()> _call;

		/// Emits a button expansion animation.
		Anim* emitExpand();
		/// Emits a button expansion animation.
		Anim* emitShrink();

		/// Starts an error animation.
		void startError();

	public:
		/// Constructs button element base.
		/// 
		/// @param map Button panel map.
		/// @param size Button size.
		Button(const Panel::Map& map, const DimVector& size);

		/// Clears button contents.
		void clear();

		/// Creates a button label element.
		/// 
		/// @param settings Label text settings.
		/// 
		/// @return Label element.
		ui::Text* setLabel(const TextSettings& settings);

		/// Creates a button icon element.
		/// 
		/// @param texture Icon texture.
		/// @param coords Texture coordinates.
		/// 
		/// @return Label element.
		ui::Image* setIcon(const sf::Texture* texture, sf::IntRect coords);

		/// Forwards the overlay element to the top.
		void forwardOverlay();

		/// Binds a keyboard key to the button.
		/// 
		/// @param key Button key.
		void bindKey(sf::Keyboard::Key key);

		/// Adds a callback to the button.
		/// 
		/// @param call Callback function.
		void attach(std::function<void()> call);

		/// Sets a click validation callback.
		/// 
		/// @param check Validation callback.
		void validate(std::function<bool()> check);

		/// Disables the button.
		void disable();
		/// Enables the button.
		void enable();

		/// Forces a button click event.
		void force();
	};
};