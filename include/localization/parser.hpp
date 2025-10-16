#pragma once

// include dependencies
#include "token.hpp"
#include "error.hpp"
#include <stdio.h>
#include <memory>

namespace localization {
	/// Localization parser state.
	///
	/// Stores localization file stream and an error list.
	struct State {
		/// Localization file stream.
		FILE* file;
		/// Current line number.
		size_t line;
		/// Current column number.
		size_t column;
		/// Error list.
		std::vector<std::unique_ptr<Error>> list;

		/// Constructs a new parser state.
		/// 
		/// @param file Parsed file stream.
		State(FILE* file);

		/// Reports an error.
		/// 
		/// @param error Owning pointer to error.
		void report(Error* error);

		/// Reads a single character from the file.
		///
		/// If the file has ended, `\0` will be returned.
		char read();
	};

	/// Loads all raw string imports.
	/// 
	/// @param state Parser state.
	/// @param text Raw string.
	/// @param root Root section.
	/// @param local Local section.
	/// 
	/// @return Text object.
	Text loadImports(State& state, const RawText& text, const Section& root, const Section& local);

	/// Reads characters until a new line.
	/// 
	/// @param state Parser state.
	/// 
	/// @return Stripped read data.
	std::string skipToNextLine(State& state);

	/// Reads a name.
	/// 
	/// @param state Parser state.
	/// @param[in] buffer First character of the name.
	/// @param[out] buffer First character after the name.
	/// 
	/// @return Read name.
	std::string readName(State& state, char& buffer);

	/// Reads in a string from a file.
	/// 
	/// @param state Parser state.
	/// @param first First read character.
	/// 
	/// @return Parsed string or `""` if an error occured.
	RawText readString(State& state, char first);

	/// Loads localization file.
	/// 
	/// @param state Parser state.
	/// 
	/// @return Parsed root localization section.
	Section load(State& state);
};