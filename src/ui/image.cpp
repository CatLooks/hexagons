#include "ui/image.hpp"

namespace ui {
	/// Constructs a new image.
	Image::Image(sf::IntRect coords, sf::Color tint)
		: coords(coords), tint(tint)
	{
		bounds.size = coords.size;
	};

	/// Draws the image.
	void Image::drawSelf(RenderBuffer& target, sf::IntRect self) const {
		target.quad(self, coords, tint);
	};
};