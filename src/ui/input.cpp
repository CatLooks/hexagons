#include "ui/input.hpp"

namespace ui {
	/// Default cursor focus callback.
	void TextInput::defaults::cursorFocus(unsigned int) {};
	/// Default text update callback.
	void TextInput::defaults::textDisplay(const sf::String&) {};
	/// Default text update callback.
	void TextInput::defaults::textConfirm(const sf::String&) {};
	/// Default input validation callback.
	bool TextInput::defaults::validation(const sf::String&, char32_t) { return true; };

	/// Sets a new cursor focus callback.
	void TextInput::attachCursorFocus(CursorFocus callback)
		{ _f_seek = callback; };
	/// Sets a new text update callback.
	void TextInput::attachTextDisplay(TextDisplay callback)
		{ _f_draw = callback; };
	/// Sets a new text confirmation callback.
	void TextInput::attachTextConfirm(TextConfirm callback)
		{ _f_enter = callback; };
	/// Sets a new input validation callback.
	void TextInput::attachValidation(Validation callback)
		{ _f_check = callback; };

	/// @return Current cursor position.
	unsigned int TextInput::getCursor() const {
		return _cursor;
	};

	/// Sets new cursor position.
	void TextInput::setCursor(unsigned int pos) {
		_cursor = pos;
		if (_cursor > _string.getSize())
			_cursor = (unsigned int)_string.getSize();
		_f_seek(_cursor);
	};

	/// Moves cursor position.
	void TextInput::moveCursor(int shift) {
		int pos = _cursor + shift;
		setCursor(pos < 0 ? 0 : pos);
	};

	/// Inserts a character at cursor.
	void TextInput::insert(char32_t c) {
		// create new string
		sf::String next = _string;
		{
			next.insert(_cursor, c);
		};

		// validate new string
		if (_f_check(next, c)) {
			_string = next;
			_f_draw(_string);
			moveCursor(1);
		};
	};

	/// Erases a character at cursor.
	void TextInput::erase(Direction dir) {
		// ignore if nothing to erase
		if ((dir == Back && _cursor == 0) || _string.isEmpty())
			return;

		// create new string
		sf::String next = _string;
		{
			next.erase(_cursor - (dir == Back ? 1ull : 0ull));
		};

		// validate new string
		if (_f_check(next, '\b')) {
			_string = next;
			_f_draw(_string);
			moveCursor(dir == Back ? -1 : 0);
		};
	};

	/// Clears the text input.
	void TextInput::clear() {
		_string.clear();
		_cursor = 0;
		display();
	};

	/// Sets text input string.
	void TextInput::set(const sf::String& text) {
		_string = text;
		_cursor = (unsigned int)text.getSize();
		display();
	};

	/// Emits display callbacks.
	void TextInput::display() const {
		_f_draw(_string);
		_f_seek(_cursor);
	};

	/// Processes a character input event.
	bool TextInput::eventChar(const Event::CharEnter& evt) {
		// insert printable characters
		if (evt.c >= ' ') {
			insert(evt.c);
		};

		// insert text from clipboard
		if (evt.c == '\x16') {
			const sf::String& text = sf::Clipboard::getString();
			for (char32_t c : text) {
				if (c >= ' ') insert(c);
			};
		};
		return true;
	};

	/// Processes a key press event.
	bool TextInput::eventKey(const Event::KeyPress& evt) {
		// set cursor to start
		if (evt.key == sf::Keyboard::Key::Home) {
			setCursor(0);
			return true;
		};

		// set cursor to end
		if (evt.key == sf::Keyboard::Key::End) {
			setCursor(~0);
			return true;
		};

		// move cursor left
		if (evt.key == sf::Keyboard::Key::Left) {
			moveCursor(-1);
			return true;
		};

		// move cursor right
		if (evt.key == sf::Keyboard::Key::Right) {
			moveCursor(1);
			return true;
		};

		// delete forwards
		if (evt.key == sf::Keyboard::Key::Delete) {
			erase(Front);
			return true;
		};

		// delete backwards
		if (evt.key == sf::Keyboard::Key::Backspace) {
			erase(Back);
			return true;
		};

		// enter
		if (evt.key == sf::Keyboard::Key::Enter) {
			if (_f_enter) _f_enter(_string);
			return true;
		};

		// ignore
		return true;
	};

	/// Returns current string inside of text input.
	const sf::String& TextInput::get() const {
		return _string;
	};
};