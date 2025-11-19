#pragma once

// include dependencies
#include "game/values/shared.hpp"
#include "game/ui/action_button.hpp"
#include <vector>

namespace gameui {
	/// Game control panel element.
	class Panel : public ui::Panel {
	public:
		/// Action box layout.
		enum Layout {
			None, /// Preview is hidden.
			L00,  /// 0 boxes on left, 0 boxes on right.
			L10,  /// 1 boxes on left, 0 boxes on right.
			L01,  /// 0 boxes on left, 1 boxes on right.
			L11,  /// 1 boxes on left, 1 boxes on right.
			L21,  /// 2 boxes on left, 1 boxes on right.
			L12,  /// 1 boxes on left, 2 boxes on right.
			L22,  /// 2 boxes on left, 2 boxes on right.
			Count
		};

	private:
		Action* _preview;            /// Preview box.
		std::vector<Action*> _boxes; /// Action box list.
		Layout _layout;              /// Current action layout.

	public:
		/// Game panel height.
		static const ui::Dim height;
		/// Action box spacing.
		static const ui::Dim spacing;

	protected:
		/// Game control panel texture map.
		static const ui::Panel::Map texture;

	public:
		/// Constructs the game panel.
		Panel();

		/// Reconstructs panel actions.
		///
		/// @param layout Layout type.
		void construct(Layout layout);

		/// Returns a reference to action panels.
		const std::vector<Action*>& actions() const;

		/// Returns a preview panel.
		Action* preview() const;
	};
};