#include "ui/align.hpp"
#include "ui/solid.hpp"

namespace ui {
	/// Constructs an alignment container.
	Align::Align(int spacing) {
		// attach update calls
		onUpdate([=](const sf::Time& _) {
			// next element position
			float x = 0.f;

			// layout elements
			for (const auto& item : children()) {
				// set element position
				item->position().x = 0.5ps + x;

				// update position
				x += (float)(item->rect().size.x + spacing);
			};

			// center elements
			float shift = (x - spacing) / 2.f;
			for (const auto& item : children())
				item->position().x -= shift;

			// recalculate bounding boxes
			recalculate();
		});
	};
};