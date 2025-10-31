#pragma once

// include dependencies
#include "element.hpp"

namespace ui {
	/// Static image display element.
	///
	/// Displays a section of a texture.
	class Image : public Element {
	public:
		/// Rendered texture.
		const sf::Texture* texture;
		/// Texture coordinates.
		sf::IntRect coords = {};
		/// Texture tint color.
		sf::Color tint;

		/// Constructs a new image.
		/// 
		/// @param texture Drawn texture.
		/// @param coords Texture coordinates.
		/// @param tint Texture tint.
		Image(const sf::Texture* texture, sf::IntRect coords = {}, sf::Color tint = sf::Color::White);

	protected:
		/// Draws the image.
		void drawSelf(RenderBuffer& target, sf::IntRect self) const override;
	};
};