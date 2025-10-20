#pragma once

// include dependencies
#include "element.hpp"

namespace ui {
	/// Static image display element.
	class Image : public Element {
	public:
		/// Texture coordinates.
		sf::IntRect coords = {};
		/// Texture tint color.
		sf::Color tint;

		/// Constructs a new image.
		/// 
		/// @param coords Texture coordinates.
		/// @param tint Texture tint.
		Image(sf::IntRect coords = {}, sf::Color tint = sf::Color::White);

	protected:
		/// Draws the image.
		void drawSelf(RenderBuffer& target, sf::IntRect self) const override;
	};
};