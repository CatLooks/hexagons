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

All asset loading should be configured inside `src/assets.hpp`.

To declare an asset, place an asset object declaration in `assets` namespace:
```cpp
namespace assets {
	sf::Font font; // test font
	sf::Texture texture; // test texture

	// etc.
};
```

To load assets, invoke an [asset loading function](##loading-functions) inside `loadAssets()`:
```cpp
// inside `::assets`
void loadAssets() {
	loadFont("font.ttf", font);
	loadTexture("image.png", texture);
	// etc.
};
```

*Note: all loaded files should be placed inside `assets/` folder.*