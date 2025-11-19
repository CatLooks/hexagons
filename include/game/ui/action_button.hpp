#pragma once

// include dependencies
#include "ui.hpp"
#include "assets.hpp"
#include "game/values/interface.hpp"

namespace gameui {
	/// Action button element.
	class Action : public ui::Panel {
	private:
		int _hint       = -1;      /// Action hint.
		ui::Text* _text = nullptr; /// Action label.

	public:
		/// Action button size.
		static const ui::Dim size;

	protected:
		/// Action button texture maps.
		///
		/// @param id Whether the button is highlighted.
		static const ui::Panel::Map textures[2];

	public:
		/// Constructs an empty action button.
		///
		/// @param hint Hint number (by default no hint).
		Action(int hint = -1);

		/// Adds an image to the action button.
		/// 
		/// @param map Action texture.
		void addTexture(sf::IntRect map);

		/// Adds a text label to the action button.
		/// 
		/// @return Pointer to text label.
		ui::Text* addLabel();
	};
};