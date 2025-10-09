#include "ui/event.hpp"

namespace ui {
	/// Constructs an event with specified data.
	Event::Event(Variant data) : data(data) {};

	/// @return Event type ID.
	size_t Event::id() const { return data.index(); };
};