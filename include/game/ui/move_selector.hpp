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
		ui::Button* _undo {}; /// Undo a move.
		ui::Button* _redo {}; /// Redo a move.
		ui::Button* _next {}; /// Finish the turn.

		ui::Text* _undo_t {}; /// Amount of moves done.
		ui::Text* _redo_t {}; /// Amount of moves reverted.
		ui::Text* _next_t {}; /// Current turn.

		ui::AnimInt* _pending {}; /// Turn button pending text.
		int _prog = 0;            /// Animation progress counter.

		bool _enabled = true; /// Whether move selectors are active.

	public:
		/// Constructs the move selector element.
		Selector();

		/// Attaches callbacks to move selector element.
		///
		/// @param undo Undo button callback.
		/// @param redo Redo button callback.
		/// @param next Turn finish callback.
		void attach(StaticHandler undo, StaticHandler redo, std::function<bool()> next);

		/// Updates move counter.
		/// 
		/// @param moves Amount of moves done / reverted.
		void update(const std::pair<size_t, size_t>& moves) const;

		/// Enables or disables the move selectors.
		void enable(bool enabled);
	};
};