#pragma once

// include dependencies
#include "ui.hpp"
#include "game/values/shared.hpp"
#include <vector>

namespace gameui {
	/// Game control panel element.
	class Panel : public ui::Panel {
	public:
		/// Action box layout.
		enum Layout {
			L00 = 0x00, /// 0 boxes on left, 0 boxes on right.
			L10 = 0x10, /// 1 boxes on left, 0 boxes on right.
			L01 = 0x01, /// 0 boxes on left, 1 boxes on right.
			L11 = 0x11, /// 1 boxes on left, 1 boxes on right.
			L21 = 0x21, /// 2 boxes on left, 1 boxes on right.
			L12 = 0x12, /// 1 boxes on left, 2 boxes on right.
			L22 = 0x22, /// 2 boxes on left, 2 boxes on right.
			Count
		};

	private:
		ui::Panel* _preview;            /// Preview box.
		std::vector<ui::Panel*> _boxes; /// Action box list.
		Layout _layout;                 /// Current action layout.

	public:
		/// Game panel height.
		static const ui::Dim height;
		/// Action box size.
		static const ui::Dim box;
		/// Action box spacing.
		static const ui::Dim spacing;

	protected:
		/// Panel texture enumeration.
		enum TextureType {
			Lower,  /// Lower panel.
			Normal, /// Skill box.
			Active, /// Active skill box.
		};

		/// Game control panel texture maps.
		static const ui::Panel::Map textures[3];

		/// Box spacing table.
		///
		/// @param id Layout type.
		static const std::vector<float> spacing_table[Count];

	public:
		/// Constructs the game panel.
		Panel();

		/// Reconstructs panel actions.
		///
		/// @param layout Layout type.
		void construct(Layout layout);

		/// Returns a reference to action panels.
		const std::vector<ui::Panel*>& actions() const;
	};
};