#pragma once

// include dependencies
#include "ui.hpp"
#include "game/values/interface.hpp"
#include "game/region.hpp"

namespace gameui {
	/// Resource bar element.
	class ResourceBar : public ui::Solid {
	public:
		/// Resource bar height.
		static const ui::Dim height;

	private:
		// resource amount
		ui::Text* l_coins   {};
		ui::Text* l_income  {};
		ui::Text* l_peaches {};
		ui::Text* l_berries {};

		// resource icons
		ui::Image* i_coins   {};
		ui::Image* i_income  {};
		ui::Image* i_peaches {};
		ui::Image* i_berries {};

		// displayed region
		Region* _region {};

	public:
		/// Constructs a resource bar.
		ResourceBar();

		/// Attaches a region for displaying.
		/// 
		/// @param region Region reference.
		void attachRegion(const Regions::Ref& region);
		/// Detaches a region from displaying.
		void detachRegion();
	};
};