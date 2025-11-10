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