#include "ui/panel.hpp"

namespace ui {
	/// Draws the panel.
	void Panel::drawSelf(RenderBuffer& target, sf::IntRect self) const {
		target.quad(self, {}, color);
	};
};