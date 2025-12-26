#pragma once

// include dependencies
#include "game/values/interface.hpp"
#include "game/ui/action_button.hpp"
#include "game/ui/move_selector.hpp"
#include <vector>

namespace gameui {
	/// Game control panel element.
	class Panel : public ui::Panel {
	private:
		Action* _preview;                  /// Preview box.
		std::vector<Action*> _boxes;       /// Action box list.
		Selector* _select;                 /// Move selector buttons.
		logic::SkillArray::Layout _layout; /// Current action layout.

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
		///
		/// @param history Map history.
		Panel(const History& history);

		/// Reconstructs panel actions.
		///
		/// @param layout Layout type.
		void construct(logic::SkillArray::Layout layout);

		/// Updates action timers.
		///
		/// @param timers Entity timer array.
		void timers(uint8_t* timers);

		/// Returns preview box.
		Action* preview() const;

		/// Returns a reference to action panels.
		const std::vector<Action*>& actions() const;

		/// Returns move selector container element.
		Selector* selector() const;
	};
};