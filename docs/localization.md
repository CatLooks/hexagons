# Localization

Localization is used to define a same dictionary of text values for multiple languages.

<hr>

## TLML file syntax

If a `#` character is encountered, everything to next newline is ignored.
```py
# comment, this is ignored
```

### Text definition

Text is a value that can be displayed with `ui::Text`.

To define a text value, use:
```py
name: text # name = "text"
```

Text values can be:
* unquoted - `name: value` *(spaces are trimmed)*.
* single-quoted - `name: 'value'`.
* double-quoted - `name: "value"`.

### Section definition

Section is a dictionary (key-value pairs) of other sections or text values.

To define a section, use:
```py
section {
	text: value
	section {
		# ...
	}
	# ...
}
```

### Value paths

To reference a value, paths are used.

Paths are constructed from a series of section accesses, separated by a dot `.`.

Example:
```py
global: Global       # global = "Global"

sub {
	local: Local     # sub.local = "Local"
	sub {
		inner: Inner # sub.sub.inner = "Inner"
	}
}
```

### Text importing

Text values can use other, already defined text values in their definition.

To define a text import, use: `$(...)`.

If a path is prefixed with `@`, the search will start from *local* scope instead of gloabl scope.

If an import failed, a replacement string will be used:
* `$(path?)` - last access in path failed.
* `$(path.*)` - path leads to a section instead of text.

Example:
```py
global: Global
sub {
	local: Local
	sub {
		inner: Inner
	}

	# [ tests ]
	a: "$(global)"     # = "Global"
	b: "$(sub.local)"  # = "Local"
	c: "$(@local)"     # = "Local"
	d: "$(@sub.inner)" # = "Inner"

	# [ errors ]
	e: "$(sub.bad)"    # = "$(sub.bad?)" (bad is not found)
	f: "$(sub)"        # = "$(sub.*)" (sub is a section)
}
```

*Note: text importing is only possible in quoted text values.*

### Text parameters

Text parameters are places in text that can be replaced by values given by the program.

To define a parameter, use: `{param_name}`.

*Note: text parameters is only possible in quoted text values.*

<hr>

## Localization usage

When constructing a `ui::Text` element, provide a [path](###value-paths) to a text value.
If a path is invalid, a replacement string will be used (`!(...)`, same as [text imports](###text-importing)).

### Setting text parameter values

| `ui::Text` method | Description |
|-|-|
| `param(name, value)` | Sets the text value of a parameter `name`. Value will be stored until overwritten or cleared. |
| `paramClear()` | Clears all stored parameter values. *This does not remove parameter hooks.* |
| `paramHook(name, hook)` | Sets up a generator hook that generates parameter `name` value before recalculation. |
| `paramHookClear()` | Clears all parameter hooks. *This does not remove stored parameter values.* |
| `hook(func)` | Sets up a procedure hook that gets called after parameter hooks. |
| `hookClear()` | Clears all procedure hooks. *This does not remove stored parameter values.* |

Usage example:
* TLML file:
```py
path: "x = {x}, fps = {fps}, mouse = {mx}, {my}"
```
* Program:
```cpp
// by default string is "x = {x}, fps = {fps}, mouse = {mx}, {my}"
ui::Text* text = new ui::Text(settings, "path");

// sets static parameter value
text->param("x", "1");
// string is "x = 1, fps = {fps}"

// parameter value hook
sf::Clock clock;
text->paramHook("fps", [=]() -> ui::Text::Hook {
	// sets "fps" every frame
	return std::format("{}", 1 / clock.restart().asSeconds());
});

// procedure hook
text->hook([=, &win]() {
	// evaluate structure value
	sf::Vector2i mouse = sf::Mouse::getPosition(win);

	// set each parameter manually
	text->param("mx", mouse.x);
	text->param("my", mouse.y);
});
```

Text parameters can also be updated 

### Parameter value imports

Parameter values import values from the global scope.

To import a path, set the parameter to `@!path`.
*Note: if an imported text value contains parameters, they will not be set*.

Importing can be escaped using `@!:` syntax, which will turn to `@!` and append text after it.

If an import fails, import value will be inserted into the string.

Usage example:
* TLML file:
```py
entity: "World"
message: "Hello, {target}!"
```
* Program:
```cpp
// by default string is "Hello, {target}!"
ui::Text* text = new ui::Text(settings, "message");

// parameter value import
text->param("target", "@!entity");
// string is "Hello, World!"

// parameter value import escape
text->param("target", "@!:entity"); // "@!:" -> "@!"
// string is "Hello, @!entity!"

// failed import
text->param("target", "@!bad");
// string is "Hello, @!bad!"
```