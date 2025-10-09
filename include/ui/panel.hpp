#pragma once

// include dependencies
#include "element.hpp"

namespace ui {
	/// Solid color panel element.
	class Panel : public Element {
	public:
		sf::Color color; /// Panel color.

	protected:
		/// Draws the panel.
		/// 
		/// @param target Render buffer.
		/// @param self Panel draw area.
		void drawSelf(RenderBuffer& target, sf::IntRect self) const override;
	};
};