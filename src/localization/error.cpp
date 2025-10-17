#include "localization/error.hpp"
#include "localization/parser.hpp"

namespace localization {
	/// Default print function for errors.
	void Error::no_output(FILE*) {};

	/// Constructs an empty error.
	Error::Error() {};
	/// Constructs an error with defined print function.
	/// 
	/// @param print Printer function.
	Error::Error(std::function<void(FILE*)> print) : print(print) {};

	/// Sets error position.
	Error* Error::at(size_t line, size_t column) {
		this->line = line;
		this->column = column;
		return this;
	};
	/// Sets error position.
	Error* Error::at(const State& state) {
		this->line = state.line;
		this->column = state.column;
		return this;
	};

	// ==== error printers ==== //

	Error* gInvalidCharacter(char c) {
		return new Error([=](FILE* out) {
			fprintf(out, "encountered an invalid character '%c' (0x%x)", c, c);
		});
	};
	Error* gUnknownEscape(char c) {
		return new Error([=](FILE* out) {
			fprintf(out, "encountered an invalid escape sequence '\\%c'", c);
		});
	};
	Error* gUnclosedString() {
		return new Error([=](FILE* out) {
			fprintf(out, "string is unclosed");
		});
	};
	Error* gMissingParamName() {
		return new Error([=](FILE* out) {
			fprintf(out, "parameter name is missing");
		});
	};
	Error* gUnclosedParam(std::string name) {
		return new Error([=](FILE* out) {
			fprintf(out, "definition of parameter \"%s\" is not closed", name.c_str());
		});
	};
	Error* gUnclosedImport() {
		return new Error([=](FILE* out) {
			fprintf(out, "import definition is unclosed");
		});
	};
	Error* gEmptyImport(Path path) {
		return new Error([=](FILE* out) {
			fprintf(out, "missing key in import path \"");

			// replacement string
			const char* repl = "?";

			// print import steps
			for (const auto& sub : path.sub) {
				fprintf(out, "%s.", sub.empty() ? repl : sub.c_str());
			};
			fprintf(out, "%s\"", path.key.empty() ? repl : path.key.c_str());
		});
	};
	Error* gInvalidImport(Path path, size_t step) {
		return new Error([=](FILE* out) {
			fprintf(out, "failed to import from path \"");

			// print import steps
			size_t left = step;
			for (const auto& sub : path.sub) {
				fprintf(out, (left == 1 ? "%s" : "%s."), sub.c_str());
				if (--left == 0) break;
			};
			if (left) fprintf(out, "%s\"", path.key.c_str());
		});
	};
	Error* gEmptyValue(std::string key) {
		return new Error([=](FILE* out) {
			fprintf(out, "missing value for key \"%s\"", key.c_str());
		});
	};
	Error* gMissingColon() {
		return new Error([=](FILE* out) {
			fprintf(out, "missing colon between key & value");
		});
	};
	Error* gUnexpectedString() {
		return new Error([=](FILE* out) {
			fprintf(out, "a key was expected, got string");
		});
	};
	Error* gUnexpectedToken() {
		return new Error([=](FILE* out) {
			fprintf(out, "unexpected token");
		});
	};
	Error* gUnexpectedSection() {
		return new Error([=](FILE* out) {
			fprintf(out, "unexpected section");
		});
	};
	Error* gStrayEndBracket() {
		return new Error([=](FILE* out) {
			fprintf(out, "unexpected closing parenthesis");
		});
	};
	Error* gEndAfterDefinition() {
		return new Error([=](FILE* out) {
			fprintf(out, "section closing bracket must be on a separate line");
		});
	};
	Error* gDuplicateKey(std::string key) {
		return new Error([=](FILE* out) {
			fprintf(out, "duplication of key \"%s\"", key.c_str());
		});
	};
	Error* gUnclosedSection(std::string name) {
		return new Error([=](FILE* out) {
			fprintf(out, "section \"%s\" is unclosed", name.c_str());
		});
	};
};