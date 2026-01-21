#include "game/loader.hpp"
#include "game/serialize/map.hpp"
#include <fstream>
#include <format>

#ifndef MAP_PATH
#define MAP_PATH "./maps/"
#endif

/// Map folder.
const fs::path Loader::folder = MAP_PATH;

/// Loads a map from a file.
std::optional<Loader::File> Loader::load(fs::path path) {
	// get filename
	std::string name = path.filename().generic_string();
	{
		// remove extension
		size_t last = name.find_last_of(".");
		if (last != std::string::npos)
			name = name.substr(0, last);
	};

	// open file
	std::ifstream str(path, std::ios::binary);
	if (!str) {
		fprintf(stderr, "[Loader] failed to open map file <%s>\n", name.c_str());
		return {};
	};

	// read file contents
	std::vector<char> buffer = std::vector<char>(
		std::istreambuf_iterator<char>(str),
		std::istreambuf_iterator<char>()
	);
	str.close();

	// deserialize file data
	sf::Packet packet;
	packet.append(buffer.data(), buffer.size());
	if (!Serialize::decodeSignature(packet)) {
		fprintf(stderr, "[Loader] signature check failed for <%s>\n", name.c_str());
		return {};
	};

	// return map data
	return File{ name, Serialize::from<Template>(packet) };
};

/// Saves a map to a file.
bool Loader::save(const File& file) {
	// fail if filename is empty
	if (file.name.empty()) return false;

	// create map folder if one does not exist
	if (!fs::is_directory(folder) || !fs::exists(folder))
		fs::create_directory("maps/");

	// attempt to open the file
	std::ofstream str(std::format("maps/{}.dat", file.name), std::ios::out | std::ios::binary);
	if (str.fail()) return false;

	// serialize template
	using namespace Serialize;
	sf::Packet packet;
	encodeSignature(packet);
	packet << file.temp;

	// write to the file
	str.write((const char*)packet.getData(), packet.getDataSize());
	str.close();
	return true;
};

/// Reloads the list.
void Loader::reload() {
	// clear list
	list.clear();

	// check if folder exists
	if (!fs::exists(folder)) {
		fprintf(stderr, "[Loader] map folder does not exist\n");
		return;
	};

	// check if folder is a directory
	if (!fs::is_directory(folder)) {
		fprintf(stderr, "[Loader] map folder is not a directory\n");
		return;
	};

	// iterate through all files
	for (const auto& file : fs::directory_iterator(folder)) {
		// try to load each file
		auto info = load(file.path());
		if (info) list.push_back(*info);
	};
};