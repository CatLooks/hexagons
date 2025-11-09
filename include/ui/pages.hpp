#pragma once

// include dependencies
#include "ui/element.hpp"

namespace ui {
	/// Page collection element.
	/// Updates only 1 child element at a time.
	/// 
	/// Note: system elements are updated & drawn regularly.
	class Pages : public Element {
	private:
		/// Currently shown element.
		Element* _current = nullptr;

	public:
		/// Constructs a paging element.
		///
		/// By default, no element is shown.
		Pages();

		/// Sets shown element.
		/// 
		/// If specified element is not in page collection, nothing will be shown.
		void show(Element* element);
	};
};