#pragma once

// include dependencies
#include "game/values/interface.hpp"
#include "assets.hpp"
#include "ui.hpp"
#include <deque>

namespace gameui {
	/// Game chat element.
	class Chat : public ui::Element {
	private:
		/// Chat message element.
		struct Message : public ui::Solid {
			ui::Text*    auth; /// Message author.
			ui::Text*    cont; /// Message contents.
			size_t      lines; /// Line count.
			sf::Time     life; /// Amount of time message exists.
			ui::AnimDim* move; /// Move animation.

			/// Constructs a message element.
			///
			/// @param height Message height.
			Message(ui::Dim height);

			/// Fades in the message.
			void fadeIn();
			/// Fades out the message.
			///
			/// Upon finishing the animation, deletes the message object.
			void fadeOut();
			/// Moves up the message.
			///
			/// @param offset Move offset.
			void moveUp(ui::Dim offset);
		};

		/// Chat message callback.
		std::function<void(const std::string& text)> _call;

		ui::Element*    _list; /// Message container.
		ui::TextField* _field; /// Input field.
		ui::Dim         _line; /// Message line height.
		bool          _active; /// Whether the chat is active.

		/// Message list.
		std::deque<Message*> _msg;
		/// Max message count.
		size_t _max;

	public:
		/// Constructs the game chat.
		///
		/// @param input Chat input height.
		/// @param msg Message line height.
		/// @param len Max chat message length.
		/// @param max Max message count.
		Chat(ui::Dim input, ui::Dim msg, size_t len, size_t max);

		/// Shows the chat input field.
		void show();
		/// Hides the chat input field.
		void hide();

		/// Writes a message to the chat.
		/// 
		/// @param author Message author.
		/// @param color Author color.
		/// @param text Message text.
		void print(const std::string& author, sf::Color color, const std::string& text);

		/// Attaches a chat message send callback.
		/// 
		/// @param callback Callback function.
		void attach(std::function<void(const std::string&)> callback);

		/// Checks whether the chat input field is active.
		bool active() const;
	};
};