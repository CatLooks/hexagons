#pragma once

// include dependencies
#include "text.hpp"
#include "input.hpp"
#include "panel.hpp"
#include "ui/anim/setter.hpp"
#include "window.hpp"

namespace ui {
	/// Simple text input field element.
	class TextField : public Panel {
	protected:
		ui::Text* _label;   /// String display.
		ui::Solid* _cursor; /// Cursor bar.

	public:
		TextInput input;    /// Input manager.
		bool focused;       /// Whether the field is accepting input.

	public:
		/// Constructs a text field element.
		///
		/// @param map Field panel map.
		/// @param settings Text settings.
		/// @param cursor
		TextField(const Panel::Map& map, const TextSettings& settings, sf::Color cursor);
	};
};