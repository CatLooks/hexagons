#pragma once

// include dependencies
#include "game/values/interface.hpp"
#include "game/ui/action_button.hpp"
#include <vector>

namespace gameui {
	/// Game control panel element.
	class Panel : public ui::Panel {
	private:
		Action* _preview;                   /// Preview box.
		std::vector<Action*> _boxes;        /// Action box list.
		Values::SkillArray::Layout _layout; /// Current action layout.

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
		void construct(Values::SkillArray::Layout layout);

		/// Returns preview box.
		Action* preview() const;

		/// Returns a reference to action panels.
		const std::vector<Action*>& actions() const;
	};
};