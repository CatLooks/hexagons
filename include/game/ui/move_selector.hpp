#pragma once

// include dependencies
#include "game/values/interface.hpp"
#include "ui.hpp"

namespace gameui {
	/// Move selector element.
	class Selector : public ui::Element {
	public:
		/// Move selector button texture map.
		static const ui::Panel::Map texture;

		/// Selector button width.
		static const ui::Dim width;

	private:
		ui::Button* _undo {}; /// Amount of moves done.
		ui::Button* _redo {}; /// Amount of moves reverted.

		ui::Text* _undo_t {}; /// Amount of moves done.
		ui::Text* _redo_t {}; /// Amount of moves reverted.

	public:
		/// Constructs the move selector element.
		Selector();

		/// Attaches callbacks to move selector element.
		///
		/// @param undo Undo button callback.
		/// @param redo Redo button callback.
		void attach(StaticHandler undo, StaticHandler redo);

		/// Updates move counter.
		/// 
		/// @param moves Amount of moves done / reverted.
		void update(const std::pair<size_t, size_t>& moves) const;
	};
};