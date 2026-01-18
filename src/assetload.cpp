#include "assetload.hpp"
#include <cstring>

namespace assets {
	/// Returns asset path.
	std::string path(std::string filename) {
		return std::string(ASSET_PATH) + filename;
	};

	// initialize all global variables
	namespace lang {
		Section locale;
		std::string standard;
		std::map<std::string, Config> index;


		std::vector<std::string> keys;
        int current_idx = 0;
        std::vector<std::function<void()>> refresh_listeners;

        /// Call this once at game startup
		void init() {
            if (loadLanguageList()) return;            
            keys.clear();
            for (auto const& [key, val] : index) {
                keys.push_back(key);
                // Set default index if it matches the 'default' key in langs.tlml
                if (key == standard) current_idx = (int)keys.size() - 1;
            }

            // Load the actual text data for the current language
            if (!keys.empty()) {
                loadLanguage(index[keys[current_idx]].file);
            }
        }

		/// Switches to the next available language and triggers menu refresh.
        void next() {
            if (keys.empty()) return;
            current_idx = (current_idx + 1) % keys.size();
            loadLanguage(index[keys[current_idx]].file);

            // Trigger every registered menu to refresh
            for (auto& refreshFunc : refresh_listeners) {
                if (refreshFunc) refreshFunc();
            }
        }


        /// Returns the display name of the current language (e.g. "English")
        std::string current_name() {
            if (keys.empty()) return "None";
            return index[keys[current_idx]].name;
        }

	};
	bool error = false;

	/// Loads a font from an asset file.
	void loadFont(std::string filename, sf::Font& font) {
		filename = path(filename);
		if (!font.openFromFile(filename)) {
			fprintf(stderr, "failed to load font <%s>\n", filename.c_str());
			error = true;
		};
	};
	/// Loads a texture from an asset file.
	void loadTexture(std::string filename, sf::Texture& texture) {
		filename = path(filename);
		if (!texture.loadFromFile(filename)) {
			fprintf(stderr, "failed to load texture <%s>\n", filename.c_str());
			error = true;
		};
	};
	/// Loads sound buffer from an asset file.
	void loadSound(std::string filename, sf::SoundBuffer& buffer, sf::Sound* sound) {
		filename = path(filename);
		if (!buffer.loadFromFile(filename)) {
			fprintf(stderr, "failed to load sound <%s>\n", filename.c_str());
			error = true;
		}
		// attach buffer to sound
		else if (sound)
			sound->setBuffer(buffer);
	};

	/// Loads language list file.
	bool loadLanguageList() {
		std::string _path = path("langs.tlml");

		// open file stream
		FILE* file = fopen(_path.c_str(), "r");
		if (!file) {
			fprintf(stderr, "failed to open <%s>: %s\n", _path.c_str(), strerror(errno));
			return true;
		};

		// load file data
		State state(file);
		Section root = load(state);
		fclose(file);

		// print errors
		for (const auto& err : state.list) {
			fprintf(stderr, "error at line %zu column %zu: ", err->line, err->column);
			err->print(stderr);
			fprintf(stderr, "\n");
		};
		if (!state.list.empty()) return true;

		// load registry data
		lang::standard = root.req("default").get({});
		if (const Value* value = root.get(std::string("langs"))) {
			if (const auto* list = std::get_if<std::unique_ptr<Section>>(value)) {
				// load each entry
				for (const auto& entry : list->get()->items) {
					lang::Config config = {
						root.req(std::format("langs.{}.name", entry.key)).get({}),
						root.req(std::format("langs.{}.file", entry.key)).get({})
					};
					lang::index[entry.key] = config;
				};
				return false;
			}
			else {
				// "langs" is a text value
				fprintf(stderr, "<langs> must be a section, not a text value\n");
				return true;
			};
		}
		else {
			// langs not found
			fprintf(stderr, "section <langs> not found\n");
			return true;
		};
	};

	/// Loads language localization file.
	bool loadLanguage(std::string filename) {
		std::string _path = path(filename);

		// open file stream
		FILE* file = fopen(_path.c_str(), "r");
		if (!file) {
			fprintf(stderr, "failed to open <%s>: %s\n", _path.c_str(), strerror(errno));
			return true;
		};

		// load file data
		State state(file);
		Section root = load(state);
		fclose(file);

		// print errors
		for (const auto& err : state.list) {
			fprintf(stderr, "error at line %zu column %zu: ", err->line, err->column);
			err->print(stderr);
			fprintf(stderr, "\n");
		};
		if (!state.list.empty()) return true;

		// success
		lang::locale = std::move(root);
		return false;
	};
};