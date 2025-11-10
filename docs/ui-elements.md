## `ui::Solid` - single color panel

Panel color is configured using field `color: sf::Color`.

<hr>

## `ui::Image` - texture section display

Image is used to display a certain part of the layer texture.

Image settings:
- `texture: sf::Texture*` - displayed texture.
- `coords: sf::IntRect` - coordinates of displayed texture part.
- `tint: sf::Color` - tint color for the texture.

<hr>

## `ui::Panel` - textured panel

Panel uses scaled textures to create a panel texture of any size.

### `ui::Panel::Map` - panel texture map

Panel configuration is done with `ui::Panel::Map` structure.

- `texture: sf::Texture*` - texture containing panel texture data.
- `corner_coords: sf::Vector2i[4]` - coordinates of corner panel parts (*order: top-left, top-right, bottom-left, bottom-right*).
- `edge_coords: sf::Vector2i[4]` - coordinates of edge panel parts (*order: top, bottom, left, right*).
- `corner_size: sf::Vector2i` - size of *all* corner panel parts.
- `edge_size: int[2]` - height of horizontal edges (`[0]`) & width of vertical edges (`[1]`).
- `middle: sf::IntRect` - coordinates and size of the middle panel part.
- `scale: int` - by default, the size of textures is used to directly constructs parts in UI space of the same size. `scale` provides a way to increase their size.

A map is needed to construct a panel (`Panel(const Map& map)`). Map data is *copied* during construction.

<hr>

## `ui::Pages` - paging element

Pages only display 1 child element at a time.

To select current shown element, use `show(Element* elem)`. If the element specified isn't found, nothing will be displayed.

*Note: system elements aren't subject to this change.*

<hr>

## `ui::Text` - text display

Text element uses localized text to display labels.

### Construction

A text element is constructed using `ui::TextSettings` and localized text path.

`ui::TextSettings` contains:
- `font` - text font.
- `size` - text size.
- `fill` - text fill color (by default white).
- `outline` - text outline color (by default black).
- `thickness` - text outline thickness (by default 0).

### Raw vs Localized text

Text elements can display text which is directly assigned to it, or by querying a currently loaded locale.

Localization methods:
* `Text(const TextSettings& settings, const localization::Path& path)` - constructs a text from a localization path.
* `setPath(const localization::Path& path)` - sets a new localization path.

Direct ("raw") methods:
* `Text::raw(const TextSettings& settings, const sf::Strign& string) -> Text` - constructs a text from a raw string.
* `setRaw(const sf::Strign& string)` - sets a new raw string.

Text elements can switch between these modes at any time.

### Placement of text in the bounding box

Text does not necessarily have the same size as the bounding box. For this reason, the placement of text inside it is controlled by alignment `align` and positioning `pos` fields.

* Alignment `align` - controls the text *alignment direction*.
  Alignment can be specified using `Up, Down, Left, Right` or `N, S, W, E` directions, or their combinations.

* Positioning `pos` - controls how the text box behaves.
  - Horizontal positioning: `None` (size depends on label text) or `ShrinkX` (removes kerning before first character).
  - Vertical positioning: `None`, `ShrinkY`, or `Static` (sets height to static line spacing).

* Label size can be set to actual text bounds using a flag `autosize`.

### Text parameters

By default, text parameters are replaced with a string `{param_name}`.

To set parameter values, use:
- `param(std::string name, std::string value)` - sets the value of parameter `name` to a string `value`.
- `paramHook(std::string name, Hook() f)` - creates a hook that evaluates the value of parameter `name` right before evaluating new text string.
  `Hook` is an optional `std::string`, if a `nullopt` is returned, the value of the parameter will not change.
- `hook(void() callback)` - creates a general hook that is evaluated before evaluating the text string.

To clear parameter values, use:
- `paramClear()` - clear all stored parameters.
- `paramHookClear()` - clear all parameter hooks.
- `hookClear()` - clear all general hooks.

### Text drawing configuration

| Function | Description |
|-|-|
| `setSize(unsigned int size)` | Sets text character size. |
| `setScale(float scale)` | Sets text scaling factor. |
| `setColor(sf::Color color)` | Sets text fill color. |
| `setOutline(sf::Color color, float thickness` | Sets text outline color and thickness. |