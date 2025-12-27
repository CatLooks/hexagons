# Abstract animation

`ui::Anim` is an abstract animation object that stores common animation behavior.

Animations are stored inside UI elements, and are managed and ticked during element recalculation.

Abstract animation object described shared animation state:
* Time management.
* Animation progress interpolation.
* Animation ending actions.

To start an animation, `push()` the animation object into an element:
```cpp
ui::Anim* anim = new ...;
element.push(anim);
```

To check if any animations are still running, use `animated()`:
```cpp
if (!element.animated()) {
	ui::Anim* anim = new ...;
	element.push(anim);
};
```

Due to internal animation ordering, animations that were created later take priority (this applies to animations that animate the same value).

## Animation settings

| Field | Type | Description |
|-|-|-|
| `ease` | `(float) -> float` | Pointer to interpolation parameter easing function. <br> Normally, easing functions should satisfy `f(0) = 0` and `f(1) = 1` \*. |
| `reversed` | `bool` | Whether the animation should run in reverse. |
| `mode` | `None`, `Loop` or `Bounce` | `None` - animation ends normally. <br> `Loop` - animations loops after ending. <br> `Bounce` - animation loops and reverses every time. |

\* *This is not technically required. `f(1)` value can return any value to fit the animation logic.*

# Animation timer

`ui::AnimTimer` is an class that utilizes the animation framework to create a timeout callback.

Usage example:
```cpp
ui::Anim* anim = new ui::AnimTimer(sf::seconds(1), []() {
	std::cout << "1 second timeout." << std::endl;
});
element.push(anim);
```

# Linear animations

`ui::AnimLinear<T, L>` is a generic class that for animations of linear values.

Animation constructors:

| Method | Description |
|-|-|
| `AnimLinear(T* target, const T& start, const T& end, sf::Time duration)` | Creates an animation from value `start` to value `end`. |
| `static to(T* target, const T& end, sf::Time duration)` | Creates an animation to value `end` (`start` is whatever the value is currently). |
| `static move(T* target, const T& shift, sf::Time duration)` | Creates an animation that shifts the value by `shift`. |

*Note: static constructors return an owning pointer.*

Class instances of linear animations:
* `AnimInt` - animates `int` type.
* `AnimFloat` - animates `float` type.
* `AnimColor` - animates `sf::Color` type (interpolation is done in sRGB space).
* `AnimDim` - animates `ui::Dim` type.
* `AnimVector` - animates `ui::DimVector` type.

Usage example:
```cpp
ui::Anim* anim = ui::AnimDim::to(
	&element->position().y, // animates element's y position
	-1es,                   // to -element_size (i.e. off-screen)
	sf::seconds(0.5f)       // in half a second
);
anim->setAfter([=]() {
	element->deactivate();  // deactivates the element after animation
});
anim->ease = ui::Easings::sineIn; // sets easing to sine in / linear out
                                  // = instead of sharp movements, element accelerates at the start
element->push(anim);
```