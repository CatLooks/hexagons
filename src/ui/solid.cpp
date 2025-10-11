#include "ui/solid.hpp"

namespace ui {
	/// Draws the solid panel.
	void Solid::drawSelf(RenderBuffer& target, sf::IntRect self) const {
		target.quad(self, {}, color);
	};
};