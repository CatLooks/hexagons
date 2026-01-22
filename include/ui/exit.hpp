#pragma once

// include dependencies
#include "text.hpp"

namespace ui {
	/// Context exit element.
	class Exit : public Text {
	private:
		float _time = 0.f;

	public:
		/// Constructs an exit element.
		///
		/// @param callback Exit callback.
		/// @param y Label Y-position.
		/// @param key Exit key.
		Exit(StaticHandler callback, ui::Dim y, sf::Keyboard::Key key = sf::Keyboard::Key::Grave);
	};
};