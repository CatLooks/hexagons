#pragma once

// include dependencies
#include <SFML/Graphics.hpp>
#include "element.hpp"

namespace ui {
	/// Solid color panel element.
	class Panel : public Element {
	public:
		sf::Color color; /// Panel color.

	protected:
		/// Draws the panel.
		/// @param self Panel draw area.
		void drawSelf(sf::IntRect self) const override {
			sf::RectangleShape shape((sf::Vector2f)self.size);
			shape.setPosition((sf::Vector2f)self.position);
			shape.setFillColor(color);
			win.draw(shape);
		};
	};
};