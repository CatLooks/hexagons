#include "ui/panel.hpp"

namespace ui {
	/// @return Corner texture map.
	sf::IntRect Panel::Map::corner(int idx) const {
		return { corner_coords[idx], corner_size };
	};
	/// @return Edge texture map.
	sf::IntRect Panel::Map::edge(int idx) const {
		return {
			edge_coords[idx], (idx >> 1)
				? sf::Vector2i(corner_size.x, edge_size[1])
				: sf::Vector2i(edge_size[0], corner_size.y)
		};
	};

	/// Draws the panel.
	void Panel::drawSelf(RenderBuffer& target, sf::IntRect self) const {
		// calculate intermediate coordinates
		// 
		// |---|-----------|---|
		// s   0           1   -

		// for x
		int xs = self.position.x;
		int x0 = self.position.x + _map.corner_size.x * _map.scale;
		int x1 = self.position.x + self.size.x - _map.corner_size.x * _map.scale;

		// for y
		int ys = self.position.y;
		int y0 = self.position.y + _map.corner_size.y * _map.scale;
		int y1 = self.position.y + self.size.y - _map.corner_size.y * _map.scale;

		// draw corners
		target.quad({ { xs, ys }, _map.corner_size * _map.scale }, _map.corner(0), color);
		target.quad({ { x1, ys }, _map.corner_size * _map.scale }, _map.corner(1), color);
		target.quad({ { xs, y1 }, _map.corner_size * _map.scale }, _map.corner(2), color);
		target.quad({ { x1, y1 }, _map.corner_size * _map.scale }, _map.corner(3), color);

		// draw edges
		bool mid = true;
		if (x1 - x0 > 0) {
			// horizontal edges
			target.quad({ { x0, ys }, { x1 - x0, _map.edge_size[1] * _map.scale } }, _map.edge(0), color);
			target.quad({ { x0, y1 }, { x1 - x0, _map.edge_size[1] * _map.scale } }, _map.edge(1), color);
		} else mid = false;
		if (y1 - y0 > 0) {
			// vertical edges
			target.quad({ { xs, y0 }, { _map.edge_size[0] * _map.scale, y1 - y0 } }, _map.edge(2), color);
			target.quad({ { x1, y0 }, { _map.edge_size[0] * _map.scale, y1 - y0 } }, _map.edge(3), color);
		} else mid = false;
		
		// draw middle
		if (mid) target.quad({ { x0, y0 }, { x1 - x0, y1 - y0 } }, _map.middle, color);
	};

	/// Constructs a new panel.
	Panel::Panel(const Panel::Map& map) : _map(map) {
		// set default padding
		padding = {
			.left   = map.edge_size[0] * map.scale,
			.right  = map.edge_size[0] * map.scale,
			.top    = map.edge_size[1] * map.scale,
			.bottom = map.edge_size[1] * map.scale
		};
	};
};