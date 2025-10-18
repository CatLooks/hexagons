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

	/// Inserts imported text.
	void loadImport(State& state, const Path& req, Text& text, const Section& root, const Section& local) {
		// try to load import path
		const Section& used = req.local ? local : root;
		auto data = used.get(req);

		// error if path is invalid
		if (const auto* err = std::get_if<size_t>(&data)) {
			state.report(gInvalidImport(req, *err)->at(state));

			// add replacement text
			text.format.append("$(");
			text.format.append(req.string(*err));
			text.format.append("?)");
			return;
		};
		if (const auto** ptr_to_value = std::get_if<const Value*>(&data)) {
			const Value* value = *ptr_to_value;

			// check if the value is text
			if (const auto* text_value = std::get_if<std::unique_ptr<Text>>(value)) {
				Text* oth = text_value->get();

				// copy parameters
				for (const Param& param : oth->params) {
					text.params.push_back({
						param.pos + text.format.size(),
						param.key
					});
				};

				// copy format string data
				text.format.append(oth->format);
				return;
			};

			// cannot import a section
			state.report(gSectionImport(req)->at(state));
			
			// add replacement text
			text.format.append("$(");
			text.format.append(req.string());
			text.format.append(".*)");
			return;
		};
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
	Path readPath(State& state, char quote) {
		Path path;

		// load first character
		char c = state.read();
		switch (c) {
			case '@':
				// set local path
				path.local = true;
				break;
			case ')':
				// empty path
				state.report(gEmptyImport(path)->at(state));
				return path;
			default:
				// process first character
				if (isalnum(c) || c == '_')
					path.key.push_back(c);
				else
					state.report(gInvalidCharacter(c)->at(state));
				break;
		};

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
				continue;
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
	Text readString(State& state, char first, const Section& root, const Section& local) {
		Text text;

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
				Path path = readPath(state, first);

				// add import data
				loadImport(state, path, text, root, local);
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
				Section* section = new Section;
				stack.top()->items.push_back({ key, section });
				stack.push(section);
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
				stack.pop();
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
					stack.top()->items.push_back({ key, new Text(str, {}) });
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
				Text text = readString(state, c, root, *stack.top());

				// add new entry
				stack.top()->items.push_back({ key, text });
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
			delete stack.top();
			stack.pop();
			state.report(gUnclosedSection(stack.top()->items.back().key)->at(state));
		};

		// return root section
		return root;
	};
};