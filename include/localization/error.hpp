#pragma once

// include dependencies
#include "token.hpp"
#include <functional>

namespace localization {
	struct State;

	/// Base error object.
	struct Error {
		/// Default print function for errors.
		static void no_output(FILE*);
		/// Prints 

		/// Line number.
		size_t line = 0;
		/// Column number.
		size_t column = 0;
		/// Error print function.
		std::function<void(FILE*)> print = no_output;

		/// Constructs an empty error.
		Error();
		/// Constructs an error with defined print function.
		/// 
		/// @param print Printer function.
		Error(std::function<void(FILE*)> print);

		/// Sets error position.
		/// 
		/// @param line Position line number.
		/// @param column Position column number.
		/// 
		/// @return Self-reference.
		Error* at(size_t line, size_t column);
		/// Sets error position.
		/// 
		/// @param state Parser state.
		/// 
		/// @return Self-reference.
		Error* at(const State& state);
	};

	/// Encountered an invalid character.
	/// @param c Encountered character.
	Error* gInvalidCharacter(char c);
	/// Encountered an unknown escape sequence.
	/// @param c Escape character.
	Error* gUnknownEscape(char c);
	/// Encountered EOL while parsing a string.
	Error* gUnclosedString();
	/// No parameter name found.
	Error* gMissingParamName();
	/// Encountered EOS while parsing a parameter.
	/// @param name Parameter name.
	Error* gUnclosedParam(std::string name);
	/// Encountered EOS while parsing an import.
	Error* gUnclosedImport();
	/// Empty path key.
	/// @param path Import path.
	Error* gEmptyImport(Path path);
	/// Attempted to import a section.
	/// @param path Section path.
	Error* gSectionImport(Path path);
	/// Invalid path provided in an import.
	/// @param path Import path.
	/// @param step Step index that generated an error.
	Error* gInvalidImport(Path path, size_t step);
	/// Missing value in a section entry.
	/// @param key Entry key name.
	Error* gEmptyValue(std::string key);
	/// Missing a colon between key and value.
	Error* gMissingColon();
	/// Encountered a string when expecting a key.
	Error* gUnexpectedString();
	/// Encountered a token when after entry has ended.
	Error* gUnexpectedToken();
	/// Encountered a section when expecting a key.
	Error* gUnexpectedSection();
	/// Encountered section end after definition.
	Error* gEndAfterDefinition();
	/// Encountered a stray closing bracket.
	Error* gStrayEndBracket();
	/// Duplicate key declaration.
	/// @param key Entry key name.
	Error* gDuplicateKey(std::string key);
	/// Encountered EOF while parsing a section.
	/// @param name Section name.
	Error* gUnclosedSection(std::string name);
};