#include "localization/parser.hpp"
#include <stack>

namespace localization {
	/// Constructs a new parser state.
	State::State(FILE* file) : file(file), line(1), column(0) {};

	/// Reports an error.
	void State::report(Error* error) {
		list.push_back(std::unique_ptr<Error>(error));
	};

	/// Reads a single character from the file.
	char State::read() {
		// read character
		char c = fgetc(file);

		// check for EOF
		if (feof(file)) return 0;

		// update cursor
		if (c == '\n') {
			line++;
			column = 0;
		}
		else column++;

		// return read character
		return c;
	};

	/// Loads all raw string imports.
	Text loadImports(State& state, const RawText& text, const Section& root, const Section& local) {
		Text result;
		return result;
	};

	/// Reads characters until a new line.
	std::string skipToNextLine(State& state) {
		std::string text;

		// skip first whitespaces
		while (char c = state.read()) {
			if (c == '\n') return text;
			if (!isspace(c)) {
				text.push_back(c);
				break;
			};
		};

		// read other data
		size_t last_nonspace = 0;
		while (char c = state.read()) {
			if (c == '\n') break;
			text.push_back(c);
			if (!isspace(c))
				last_nonspace = text.size();
		};
		return text.substr(0, last_nonspace);
	};

	/// Reads a name.
	std::string readName(State& state, char& buffer) {
		std::string name;
		name.push_back(buffer);
		while (char c = state.read()) {
			if (!(isalnum(c) || c == '_')) {
				buffer = c;
				break;
			};
			name.push_back(c);
		};
		return name;
	};

	/// Reads in a string from a file.
	RawText readString(State& state, char first) {
		RawText text;

		// parser loop
		bool escape = false; // is '\'?
		bool import = false; // is '$'?

		while (char c = state.read()) {
			// parse escape sequence character
			if (escape) {
				switch (c) {
					case '"' : text.format.push_back('"' ); break;
					case '\'': text.format.push_back('\''); break;
					case '$' : text.format.push_back('$' ); break;
					case '{' : text.format.push_back('{' ); break;
					case '}' : text.format.push_back('}' ); break;
					case 'n' : text.format.push_back('\n'); break;
					default:
						// bad escape
						state.report(gUnknownEscape(c)->at(state));
						break;
				};
				escape = false;
				continue;
			};

			// check for escape sequence
			if (c == '\\') {
				escape = true;
				continue;
			};

			// check for string exit
			if (c == first) return text;

			// check for an import
			if (c == '(' && import) {
				// read import path
				// @todo
				import = false;
				continue;
			};

			// check for a parameter
			if (c == '{') {
				std::string key;

				// read parameter key
				while (1) {
					char k = state.read();

					// check for exit
					if (k == '}') break;

					// check for forced exit
					if (k == first || !k) {
						state.report(gUnclosedParam(key)->at(state));
						break;
					};

					// add character to key string
					key.push_back(k);
				};
				text.params.push_back(Param{ text.format.size(), key });
				continue;
			};

			// check for import prefix start
			if (c == '$') import = true;

			// add character to format string
			text.format.push_back(c);
		};

		// string is unfinished
		state.report(gUnclosedString()->at(state));
		return text;
	};

	/// Loads localization file.
	Section load(State& state) {
		// section stack
		Section root;
		std::stack<Section*> stack;
		stack.push(&root);

		// parser state
		std::string key;
		bool colon = false;
		bool inhibit_read = false;

		// parse the file
		char c = 0;
		while (1) {
			// read next character
			if (!inhibit_read) {
				c = state.read();
				if (!c) break;
			};
			inhibit_read = false;

			// ignore whitespaces
			if (isspace(c)) continue;

			// execute if a key is not entered
			if (key.empty()) {
				// skip comments
				if (c == '#') {
					skipToNextLine(state);
					continue;
				};

				// check for a key
				if (isalnum(c) || c == '_') {
					// read the key
					key = readName(state, c);
					inhibit_read = true;
					continue;
				};

				// check for a string
				if (c == '\'' || c == '"') {
					readString(state, c);
					state.report(gUnexpectedString()->at(state));
					skipToNextLine(state);
					continue;
				};
			}
			// execite of a key is entered
			else {
				// skip comments
				if (c == '#') {
					state.report(gEmptyValue(key)->at(state));
					skipToNextLine(state);
					key.clear();
					continue;
				};

				// check for a colon
				if (c == ':') {
					if (colon)
						state.report(gInvalidCharacter(c)->at(state));
					colon = true;
					continue;
				};

				// check for an unquoted value
				if (isalnum(c) || c == '_') {
					// check for missing colon
					if (!colon)
						state.report(gMissingColon()->at(state));
					colon = false;

					// read the value
					std::string data = skipToNextLine(state);

					// add a section entry
					stack.top()->items.push_back({ key, Text(data, {}) });
					key.clear();
					continue;
				};

				// check for a string
				if (c == '\'' || c == '"') {
					// read string & load imports
					RawText raw = readString(state, c);
					Text text = loadImports(state, raw, root, *stack.top());

					// add a section entry
					stack.top()->items.push_back({ key, text });
					key.clear();
					continue;
				};
			};

			// invalid character
			state.report(gInvalidCharacter(c)->at(state));
		};
		
		// return root section
		return root;
	};
};