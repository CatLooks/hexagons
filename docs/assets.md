# Assets

Assets are files that are statically loaded in during game initialization.

## Loading functions

To load assets, use the following functions:

| Function | Description |
|-|-|
| `loadFont(std::string filename, sf::Font& font)` | Loads a font. |
| `loadTexture(std::string filename, sf::Texture& tex)` | Loads a texture. |
| `loadSound(std::string filename, sf::SoundBuffer& buffer)` | Loads sound data. |
| `loadSound(std::string filename, sf::SoundBuffer& buffer, sf::Sound* sound)` | Also links sound buffer to a sound playback object. |

If a loading function fails, `assets::error` flag will be set to `true`.

## Load declarations

All assset declarations need to be placed inside `include/assets.hpp`, for example:
```cpp
#pragma once
#include "assetload.hpp"

namespace assets {
	extern sf::Font font; // example font
	extern sf::Texture texture; // example texture
	// ...
};
```

Instantiating and [loading](##loading-functions) assets needs to be done inside `src/assets.cpp`, for example:

```cpp
namespace assets {
	sf::Font font; // test font
	sf::Texture texture; // test texture

	void loadAssets() {
		loadFont("font.ttf", font);
		loadTexture("image.png", texture);
	};
};
```

*Note: all loaded files should be placed inside `assets/` folder.*