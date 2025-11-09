#include "ui/pages.hpp"

namespace ui {
	/// Constructs a paging element.
	Pages::Pages() {
		onRecalculate([=](const sf::Time& _) {
			// update activity of all public elements
			for (const auto& element : children()) {
				if (element.get() == _current)
					element->activate();
				else
					element->deactivate();
			};
		});
	};

	/// Sets shown element.
	void Pages::show(Element* element) {
		_current = element;
	};
};