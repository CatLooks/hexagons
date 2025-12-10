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
		/// Resource bar displayed data enumeration.
		enum Data {
			Money,  /// Money amount.
			Income, /// Income amount.
			Berry,  /// Berry amount.
			Peach,  /// Peach amount.
			Count
		};

		ui::Element* _resources  {}; /// Resource label space.
		ui::Align* _conts[Count] {}; /// Data containers.
		ui::Text* _labels[Count] {}; /// Data labels.
		ui::Image* _icons[Count] {}; /// Data icons.

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