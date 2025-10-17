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
		
		// process all instructions
		for (const auto& inst : text.params) {
			// pass through parameters
			if (const auto* param = std::get_if<Param>(&inst)) {
				result.params.push_back({ result.format.size(), param->key });
				continue;
			};

			// load import
			if (const auto* import = std::get_if<Import>(&inst)) {
				// try to load import path

			};
		};

		// return loaded text string
		return result;
	};

	/// Reads characters until a new line.
	void skipToNextLine(State& state) {
		while (char c = state.read()) {
			if (c == '\n') break;
		};
	};

	/// Reads characters until a new line.
	std::string readLine(State& state, char c) {
		std::string text;
		if (c) text.push_back(c);
		while (char c = state.read()) {
			if (c == '\n') break;
			text.push_back(c);
		};
		return text;
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

	/// Reads a path.
	Path readPath(State& state, char buffer, char quote) {
		Path path;

		// load first character
		if (buffer == '@')
			path.local = true;
		else
			path.key.push_back(buffer);

		// parse characters
		bool empty_error = false;
		while (char c = state.read()) {
			// add character to the key
			if (isalnum(c) || c == '_') {
				path.key.push_back(c);
				continue;
			};

			// check for subsection split
			if (c == '.') {
				if (path.key.empty()) empty_error = true;
				path.sub.push_back(path.key);
				path.key.clear();
			};

			// break if encountered a parenthesis
			if (c == ')') break;

			// force break if EOS
			if (c == quote) {
				state.report(gUnclosedImport()->at(state));
				break;
			};

			// invalid character
			state.report(gInvalidCharacter(c)->at(state));
		};

		// check for empty key
		if (path.key.empty() || empty_error)
			state.report(gEmptyImport(path)->at(state));
		return path;
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
				// remove '$' from format string
				text.format.pop_back();

				// read import path
				Path path = readPath(state, c, first);

				// add import path
				text.params.push_back(Import{ text.format.size(), path });
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
		// stack frame data
		struct data_t {
			Section* sec;    /// Section reference.
			std::string key; /// Section key.
		};

		// section stack
		Section root;
		std::stack<data_t> stack;
		stack.push({ &root, "" });

		// parser state
		std::string key;           // Entry key string.
		bool colon = false;        // ':' has been encountered.
		bool inhibit_read = false; // Don't fetch next character.
		bool newline = false;      // Process new line during next cycle.
		bool entry_end = false;    // Whether the entry has been fully parsed.
		bool queue_exit = false;   // Exit the loop during next new line check.

		// parse the file
		char c = 0;
		while (1) {
			// handle new line
			if (newline) {
				// check for unfinished entry
				if (!entry_end && !key.empty())
					state.report(gEmptyValue(key)->at(state));

				// reset flags
				key.clear();
				newline = false;
				entry_end = false;
				colon = false;

				// check for exit
				if (queue_exit) break;
			};

			// read next character
			if (!inhibit_read) {
				c = state.read();
				if (!c) {
					c = '\n';
					queue_exit = true;
				};
			};
			inhibit_read = false;

			// process newlines
			if (c == '\n') {
				newline = true;
				continue;
			};
			// ignore comments
			if (c == '#') {
				skipToNextLine(state);
				newline = true;
				continue;
			};

			// ignore whitespaces
			if (isspace(c)) continue;

			// check for a section start
			if (c == '{') {
				// check for stray section
				if (key.empty()) {
					state.report(gUnexpectedSection()->at(state));
				};

				// create new stack frame
				stack.push({ new Section, key });
				entry_end = true;
				continue;
			};

			// check for a section end
			if (c == '}') {
				// check for anything before
				if (!key.empty() || entry_end) {
					state.report(gEndAfterDefinition()->at(state));
				};

				// check for stray parenthesis
				if (stack.size() <= 1) {
					state.report(gStrayEndBracket()->at(state));
					continue;
				};

				// pop stack frame
				data_t frame = stack.top();
				stack.pop();

				// add a section entry
				stack.top().sec->items.push_back({ frame.key, frame.sec });
				entry_end = true;
				continue;
			};

			// check for a colon
			if (c == ':') {
				// check for stray colon
				if (entry_end) {
					state.report(gUnexpectedToken()->at(state));
					skipToNextLine(state);
					newline = true;
					continue;
				};

				// check for an extra colon
				if (colon)
					state.report(gInvalidCharacter(c)->at(state));
				colon = true;
				continue;
			};

			// check for a key
			if (isalnum(c) || c == '_') {
				// check for stray key
				if (entry_end) {
					state.report(gUnexpectedToken()->at(state));
					skipToNextLine(state);
					newline = true;
					continue;
				};

				// read new entry key
				if (key.empty()) {
					// read the rest of the key
					key = readName(state, c);
					inhibit_read = true;
					continue;
				};

				// check for an unquoted value
				if (colon) {
					// read the value
					std::string str = readLine(state, c);
					newline = true;

					// add new entry
					stack.top().sec->items.push_back({ key, new Text(str, {}) });
					entry_end = true;
					continue;
				};

				// unexpected value
				state.report(gUnexpectedToken()->at(state));
				continue;
			};

			// check for a string
			if (c == '\'' || c == '"') {
				// check for a stray string
				if (entry_end) {
					state.report(gUnexpectedToken()->at(state));
					skipToNextLine(state);
					newline = true;
					continue;
				};

				// read the string
				RawText raw = readString(state, c);
				// load string imports
				Text text = loadImports(state, raw, root, *stack.top().sec);

				// add new entry
				stack.top().sec->items.push_back({ key, text });
				entry_end = true;
				continue;
			};

			// invalid character
			state.report(gInvalidCharacter(c)->at(state));
			skipToNextLine(state);
			newline = true;
		};
		
		// check for unclosed sections
		while (stack.size() > 1) {
			state.report(gUnclosedSection(stack.top().key)->at(state));
			delete stack.top().sec;
			stack.pop();
		};

		// return root section
		return root;
	};
};