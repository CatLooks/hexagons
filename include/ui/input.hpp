#pragma once

// include dependencies
#include <SFML/System/String.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <functional>
#include "ui/event.hpp"

namespace ui {
	/// Text input object.
	/// Stores text string and controls its modification.
	class TextInput {
	public:
		/// Cursor focus callback.
		///
		/// @param cursor Cursor position.
		using CursorFocus = std::function<void(unsigned int cursor)>;
		/// Text display callback.
		///
		/// @param string New text string.
		using TextDisplay = std::function<void(const sf::String& string)>;
		/// Text confirmation callback.
		///
		/// @param string New text string.
		using TextConfirm = std::function<void(const sf::String& string)>;
		/// Text validation callback.
		///
		/// @param string Text string after modification.
		/// @param c Newly added character.
		/// 
		/// @return Whether the new text string is valid.
		using Validation = std::function<bool(const sf::String& string, char32_t c)>;

		/// Defaults for input callbacks.
		struct defaults {
			/// Default cursor focus callback.
			static void cursorFocus(unsigned int cursor);
			/// Default text update callback.
			static void textDisplay(const sf::String& string);
			/// Default text update callback.
			static void textConfirm(const sf::String& string);
			/// Default input validation callback.
			static bool validation(const sf::String& string, char32_t c);

			// delete this just in case
			defaults() = delete;
		};

	private:
		/// Previous string.
		sf::String _string_p;
		/// Previous cursor position.
		unsigned int _cursor_p = 0;

	protected:
		/// Current string.
		sf::String _string;
		/// Input field cursor position.
		unsigned int _cursor = 0;

		/// Cursor focus callback.
		CursorFocus _f_seek = defaults::cursorFocus;
		/// Text field update callback.
		TextDisplay _f_draw = defaults::textDisplay;
		/// Input confirmation callback.
		TextConfirm _f_enter = defaults::textConfirm;
		/// Input validation callback.
		Validation _f_check = defaults::validation;

	public:
		/// Sets a new cursor focus callback.
		///
		/// This callback is used to make sure that the cursor is always visible.
		/// 
		/// By default, no action is taken.
		void attachCursorFocus(CursorFocus callback);
		/// Sets a new text update callback.
		/// 
		/// This callback is used to update text on screen.
		///
		/// By default, no action is taken.
		void attachTextDisplay(TextDisplay callback);
		/// Sets a new text confirmation callback.
		/// 
		/// This callback is used to process input when `Enter` is pressed.
		///
		/// By default, no action is taken.
		void attachTextConfirm(TextConfirm callback);
		/// Sets a new input validation callback.
		/// 
		/// This callback is used to check whether a new input string is valid.
		/// 
		/// When a part of a string is erased, the character passed is `\b`.
		///
		/// By default, no action is taken.
		void attachValidation(Validation callback);

		/// @return Current cursor position.
		unsigned int getCursor() const;
		/// Sets new cursor position.
		/// @param pos Cursor position.
		void setCursor(unsigned int pos);
		/// Moves cursor position.
		/// @param shift Cursor position shift.
		void moveCursor(int shift);

		/// Erase direction.
		enum Direction {
			Back, /// Erase backwards (`Backspace` key).
			Front /// Erase forwards (`Delete` key).
		};

		/// Inserts a character at cursor.
		/// 
		/// Inserted character must be a part of the string.
		/// 
		/// @param c Character codepoint.
		void insert(char32_t c);
		/// Erases a character at cursor.
		///
		/// @param dir Erase direction.
		void erase(Direction dir);

		/// Emits display callbacks.
		void display() const;

		/// Processes a character input event.
		/// 
		/// @param evt Character input event data.
		/// 
		/// @return `true` (return value for event handler).
		bool eventChar(const Event::CharEnter& evt);
		/// Processes a key press event.
		/// 
		/// @param evt Key press event data.
		/// 
		/// @return `true` (return value for event handler).
		bool eventKey(const Event::KeyPress& evt);
	};
};