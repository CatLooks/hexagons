#include "assetload.hpp"

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
		FILE* file;
		if (int err = fopen_s(&file, _path.c_str(), "r")) {
			fprintf(stderr, "failed to open <%s>: %s\n", _path.c_str(), strerror(err));
			return true;
		};

		// load file data
		State state(file);
		Section root = load(state);
		fclose(file);

		// print errors
		for (const auto& err : state.list) {
			fprintf(stderr, "error at line %llu column %llu:", err->line, err->column);
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
		FILE* file;
		if (int err = fopen_s(&file, _path.c_str(), "r")) {
			fprintf(stderr, "failed to open <%s>: %s\n", _path.c_str(), strerror(err));
			return true;
		};

		// load file data
		State state(file);
		Section root = load(state);
		fclose(file);

		// print errors
		for (const auto& err : state.list) {
			fprintf(stderr, "error at line %llu column %llu:", err->line, err->column);
			err->print(stderr);
			fprintf(stderr, "\n");
		};
		if (!state.list.empty()) return true;

		// success
		lang::locale = std::move(root);
		return false;
	};
};