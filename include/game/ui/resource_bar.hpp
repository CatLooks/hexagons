#pragma once

// include dependencies
#include "ui.hpp"

namespace gameui {
	/// Resource bar element.
	class ResourceBar : public ui::Solid {
	private:
		ui::Text* l_coins;
		ui::Text* l_income;
		ui::Text* l_peaches;
		ui::Text* l_berries;

		ui::Image* i_coins;
		ui::Image* i_income;
		ui::Image* i_peaches;
		ui::Image* i_berries;

	public:
		/// Constructs a resource bar.
		ResourceBar();
	};
};