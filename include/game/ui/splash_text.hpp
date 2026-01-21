#pragma once

// include dependencies
#include "game/values/interface.hpp"
#include "assets.hpp"
#include "ui.hpp"

namespace gameui {
	/// Splash text element.
	class Splash : public ui::Panel {
	public:
		/// Splash background panel map.
		static const ui::Panel::Map texture;

		/// Shared argument list.
		ui::Text::List args;

	private:
		/// Text element list.
		std::deque<std::vector<std::unique_ptr<ui::Text>>> _text;
		/// Text offset.
		ui::Dim _offset;
		/// Whether to block propagation of mouse events.
		bool _inhibit = false;
		/// Display wait animation.
		ui::Anim* _wait = nullptr;

		/// Fades out the panel.
		void fade();

		/// Starts next text block display.
		void next(ui::Align* _prev, ui::Align* _next);

	public:
		/// Constructs an empty splash text element.
		///
		/// @param height Splash panel height.
		/// @param text Splash text offset.
		Splash(ui::Dim height, ui::Dim text);

		/// Queues a text label for display.
		/// 
		/// Arguments are stored in `this->args`.
		/// 
		/// @param path Text localization path.
		/// @param color Text color.
		void queue(const localization::Path& path, sf::Color color = sf::Color::White);

		/// Creates a new splash frame.
		void frame();

		/// Displays queued text labels.
		void display();
	};
};