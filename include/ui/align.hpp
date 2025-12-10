#pragma once

// include dependencies
#include "element.hpp"

namespace ui {
	/// Horizontal item alignment container.
	/// Aligns all elements in the middle of the container.
	class Align : public Element {
	public:
		/// Constructs an alignment container.
		///
		/// @param spacing Spacing between aligned elements.
		Align(int spacing = 0);
	};
};