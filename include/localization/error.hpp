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
	/// @return New error object.
	Error* gInvalidCharacter(char c);
	/// Encountered an unknown escape sequence.
	/// @param c Escape character.
	/// @return New error object.
	Error* gUnknownEscape(char c);
	/// Encountered EOL while parsing a string.
	/// @return New error object.
	Error* gUnclosedString();
	/// No parameter name found.
	/// @return New error object.
	Error* gMissingParamName();
	/// Encountered EOS while parsing a parameter.
	/// @param name Parameter name.
	/// @return New error object.
	Error* gUnclosedParam(std::string name);
	/// Invalid path provided in an import.
	/// @param path Import path.
	/// @param step Step index that generated an error.
	/// @return New error object.
	Error* gInvalidImport(Path path, size_t step);
	/// Missing value in a section entry.
	/// @param key Entry key name.
	/// @return New error object.
	Error* gEmptyValue(std::string key);
	/// Missing a colon between key and value.
	/// @return New error object.
	Error* gMissingColon();
	/// Encountered a string when expecting a key.
	/// @return New error object.
	Error* gUnexpectedString();
	/// Duplicate key declaration.
	/// @param key Entry key name.
	/// @return New error object.
	Error* gDuplicateKey(std::string key);
	/// Encountered EOF while parsing a section.
	/// @param name Section name.
	/// @return New error object.
	Error* gUnclosedSection(std::string name);
};