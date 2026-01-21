#pragma once

// include dependencies
#include "template.hpp"
#include <filesystem>

/// `std::filesystem`.
namespace fs = std::filesystem;

/// Template loader object.
struct Loader {
	/// Map folder.
	static const fs::path folder;

	/// Map file info.
	struct File {
		/// File name.
		std::string name;
		/// Map template.
		Template temp;
	};

	/// Loaded template list.
	std::list<File> list;

	/// Loads a map from a file.
	/// 
	/// @param path File path.
	/// 
	/// @return Map file data.
	static std::optional<File> load(fs::path path);

	/// Saves a map to a file.
	/// 
	/// @param file Map file info.
	/// 
	/// @return Whether the operation was successful.
	static bool save(const File& file);

	/// Reloads the map list.
	void reload();
};