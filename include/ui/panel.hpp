#pragma once

// include dependencies
#include "element.hpp"
#include "solid.hpp"

namespace ui {
	/// Textured panel element.
	class Panel : public Solid {
	public:
		/// Panel texture map.
		struct Map {
			/// Texture reference.
			const sf::Texture* texture;

			/// Coordinates of corner pieces.
			///
			/// Order: `top-left`, `top-right`, `bottom-left`, `bottom-right`.
			sf::Vector2i corner_coords[4];

			/// Coordinates of edge pieces.
			///
			/// Order: `top`, `bottom`, `left`, `right`.
			sf::Vector2i edge_coords[4];

			/// Size of corner pieces.
			sf::Vector2i corner_size;

			/// Size of edge pieces.
			///
			/// Order: `horizontal`, `vertical`.
			int edge_size[2];
			
			/// Coordinates and size of the middle piece.
			sf::IntRect middle;

			/// Corner & edge scale.
			int scale = 1;

			/// @return Corner texture map.
			/// @param idx Corner index (see `corner_coords`).
			sf::IntRect corner(int idx) const;
			/// @return Edge texture map.
			/// @param idx Edge index (see `edge_coords`).
			sf::IntRect edge(int idx) const;
		};

	protected:
		/// Panel texture map.
		Map _map;

		/// Draws the panel.
		/// 
		/// @param target Render buffer.
		/// @param self Panel draw area.
		void drawSelf(RenderBuffer& target, sf::IntRect self) const override;

	public:
		/// Constructs a new panel.
		/// @param map Panel texture map.
		Panel(const Map& map);
	};

};