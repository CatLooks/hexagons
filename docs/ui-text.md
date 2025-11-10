# Text input manager

`ui::TextInput` is an object that manages text input.

## Modification methods

| Function | Description |
|-|-|
| `insert(char32_t c)` | Insert a character at cursor. |
| `erase(Direction dir)` | Erases a character at cursor. |

## Cursor control

| Function | Description |
|-|-|
| `getCursor() -> unsigned int` | Returns cursor position. |
| `setCursor(unsigned int pos)` | Sets new cursor position. <br> If position is out-of-range, cursor will be set to the end. |
| `moveCursor(int shift)` | Shifts cursor position. |

## Event processing

| Function | Description |
|-|-|
| `eventChar(const Event::CharEnter& evt) -> bool` | Processes a character input event. <br> Returns `true`. |
| `eventKey(const Event::KeyPress& evt) -> bool` | Processes a key press event. <br> Returns `true`. |

## Manager callbacks

Each time an input state updates, a callback is invoked.

Available callbacks:

| Name | Function signature | Attach function | Description |
|-|-|-|-|
| Cursor focus | `void(unsigned int cursor)` | `attachCursorFocus(...)` | Gets invoked when cursor changes its position. |
| Text display | `void(const sf::String& string)` | `attachTextDisplay(...)` | Gets invoked when input text needs to be updated. |
| Text confirm | `void(const sf::String& string)` | `attachTextConfirm(...)` | Gets invoked when `Enter` is pressed. |
| Input validation | `bool(const sf::String& string, char32_t c)` | `attachValidation(...)` | Gets invokes when the input text gets changed. <br> If returns `false`, input text does not change. |

## Usage example

```cpp
// input display element
ui::Text* display = ui::Text::raw(settings, "");
// cursor element
ui::Text* cursor = ui::Text::raw(settings, "_");

// input manager
ui::TextInput input;

/// input validation
input.attachValidation([](const sf::String& string, char32_t c) {
	// `c` allows to discard invalid characters immediately
	// for more complicated checks, use `string`
	return isdigit(c) || c == '-'
		|| c == '\b'; // < this gets passed through when text is erased
});

/// text display update
input.attachTextDisplay([=](const sf::String& string) {
	// update input display text element
	// displayed string can be modified here
	sf::String res = string;
	size_t pos = string.getSize() - string.getSize() % 3;
	if (string.getSize() % 3 == 0) pos -= 3;
	while (pos >= 3) {
		res.insert(pos, '-');
		pos -= 3;
	};
	display->setRaw(res);
});

/// cursor focus update
input.attachCursorFocus([=](unsigned int pos) {
	// update cursor position
	cursor->position().x = text->position().x + text->charpos(pos).x;

	// this gets called after text label has been set, so
	// this can be used to shift text within the input field
});
```