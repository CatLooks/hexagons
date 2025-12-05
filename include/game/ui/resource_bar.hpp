#pragma once

// include dependencies
#include "game/values/interface.hpp"
#include "ui.hpp"
#include "game/region.hpp"

namespace gameui {
	/// Resource bar element.
	class Bar : public ui::Solid {
	public:
		/// Resource bar height.
		static const ui::Dim height;

	private:
		ui::Text* t_money   {}; /// Money text.
		ui::Text* t_income  {}; /// Income text.
		ui::Text* t_peaches {}; /// Peach text.
		ui::Text* t_berries {}; /// Berry text.

		ui::Image* i_money   {}; /// Money icon.
		ui::Image* i_income  {}; /// Income icon.
		ui::Image* i_peaches {}; /// Peach icon.
		ui::Image* i_berries {}; /// Berry icon.

		Region* _region {}; /// Displayed region.

	public:
		/// Constructs a resource bar.
		Bar();

		/// Attaches a region for displaying.
		///
		/// @param region Region reference.
		void attachRegion(const Regions::Ref& region);
		/// Detaches a region from displaying.
		void detachRegion();
	};
};