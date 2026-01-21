#pragma once

// include dependencies
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "localization/parser.hpp"
#include <vector>
#include <map>

/// Asset management namespace.
namespace assets {
	using namespace localization;

	/// Returns asset path.
	/// 
	/// @param filename File name.
	/// 
	/// @return Path to asset.
	std::string path(std::string filename);

	/// Localization namespace.
	namespace lang {
		/// Currently loaded language.
		extern Section locale;

		/// Default language key.
		extern std::string standard;

		/// Language configuration.
		struct Config {
			std::string name; /// Language display name.
			std::string file; /// Localization file name.
		};

		/// Languages key list.
		extern std::vector<std::string> list;

		/// Language config map.
		extern std::map<std::string, Config> index;

        extern int current_idx;

        /// Call this once at game startup
		void init();

		extern std::vector<std::function<void()>> refresh_listeners; /// Registered menu refresh listeners

		/// Switches to the next available language and triggers menu refresh.
		void next();

		std::string current_name();
	};

	/// Whether an error occured during asset loading.
	extern bool error;

	/// Loads all assets.
	void loadAssets();

	/// Loads a font from an asset file.
	/// 
	/// @param filename Font file name.
	/// @param font Font object.
	void loadFont(std::string filename, sf::Font& font);
	/// Loads a texture from an asset file.
	/// 
	/// @param filename Texture file name.
	/// @param texture Texture object.
	void loadTexture(std::string filename, sf::Texture& texture);
	/// Loads sound buffer from an asset file.
	/// 
	/// @param filename Sound file name.
	/// @param buffer Sound buffer object.
	/// @param sound Sound object (can be set to `nullptr` to ignore).
	void loadSound(std::string filename, sf::SoundBuffer& buffer, sf::Sound* sound = nullptr);

	/// Loads language list file.
	/// @return Whether the loading failed.
	bool loadLanguageList();

	/// Loads language localization file.
	/// 
	/// @param key Language key.
	/// 
	/// @return Whether the loading failed.
	bool loadLanguage(std::string key);
};