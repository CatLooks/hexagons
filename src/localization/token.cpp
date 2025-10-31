#include "localization/token.hpp"
#include <format>

namespace localization {
	/// Prints path parsing error data.
	/// 
	/// @param string Path string.
	/// @param i Error position.
	static void path_error(const char* string, size_t i) {
		fprintf(stderr, "\n  at %llu while parsing \"%s\"\n", i, string);
	};

	/// Constructs an empty path.
	Path::Path() {};
	/// Constructs a path from a C-string.
	Path::Path(const char* string) {
		// parse every character
		size_t i = 0;
		for (; char c = string[i]; i++) {
			// check for a local path
			if (c == '@') {
				fprintf(stderr, "local modifier '@' not allowed");
				path_error(string, i);
				return;
			};

			// check for key data
			if (isalnum(c) || c == '_') {
				key.push_back(c);
				continue;
			};

			// check for subsection separator
			if (c == '.') {
				if (key.empty()) {
					fprintf(stderr, "missing subsection name");
					path_error(string, i);
					return;
				};
				sub.push_back(key);
				key.clear();
				continue;
			};

			// unknown character
			fprintf(stderr, "unknown character '%c' (0x%x)", c, c);
			path_error(string, i);
		};

		// check for empty key
		if (key.empty()) {
			fprintf(stderr, "missing field name");
			path_error(string, i);
			return;
		};
	};
	/// Constructs a path from a string.
	Path::Path(std::string string) : Path(string.c_str()) {};

	/// Constructs a new entry.
	Entry::Entry(std::string key, Text* value)
		: key(key), value(std::unique_ptr<Text>(value)) {};
	/// Constructs a new entry.
	Entry::Entry(std::string key, Section* value)
		: key(key), value(std::unique_ptr<Section>(value)) {};
	/// Constructs a new entry.
	Entry::Entry(std::string key, Text value)
		: key(key), value(std::make_unique<Text>(std::move(value))) {};
	/// Constructs a new entry.
	Entry::Entry(std::string key, Section value)
		: key(key), value(std::make_unique<Section>(std::move(value))) {};

	/// Returns a value denoted by a key.
	const Value* Section::get(std::string key) const {
		for (auto& entry : items) {
			if (entry.key == key)
				return &entry.value;
		};
		return nullptr;
	};

	/// Returns a value denoted by a recursive path.
	std::variant<const Value*, size_t> Section::get(Path path) const {
		// route to `get` if no subsections
		if (path.sub.empty()) {
			auto value = get(path.key);
			if (value) return value;
			return (size_t)0;
		};

		// find first subsection
		for (const auto& entry : items) {
			if (entry.key == path.sub.front()) {
				// entry contains a section
				if (const auto* section = std::get_if<std::unique_ptr<Section>>(&entry.value)) {
					// remove first subsection
					path.sub.erase(path.sub.begin());

					// access further subsections
					auto value = section->get()->get(path);
					if (size_t* err = std::get_if<size_t>(&value))
						return *err + 1;
					return value;
				};

				// entry contains text
				return (size_t)0;
			};
		};

		// subsection not found
		return (size_t)0;
	};

	/// Requests the text value at a path.
	Text Section::req(const Path& path) const {
		// load path value
		auto data = get(path);

		// check for access error
		if (const size_t* err = std::get_if<size_t>(&data))
			return Text{ "!(" + path.string(*err) + "?)", {} };
		if (const Value** value_ptr = std::get_if<const Value*>(&data)) {
			// check if the value is text
			if (const auto* text = std::get_if<std::unique_ptr<Text>>(*value_ptr))
				return *text->get();

			// value is a section
			return Text{ "!(" + path.string() + ".*)", {} };
		};
		return {};
	};

	/// Creates a formatted text string.
	std::string Text::get(const std::unordered_map<std::string, std::string>& args, const Section* dict) const {
		// insert all parameters
		std::string result;
		size_t idx = 0;
		for (const Param& param : params) {
			// copy format data
			if (idx < param.pos) {
				result.append(format.substr(idx, param.pos - idx));
				idx = param.pos;
			};

			// insert parameter value
			auto value = args.find(param.key);
			if (value == args.cend())
				result.append("{" + param.key + "}");
			else {
				// check if value is '@...'
				if (value->second.size() >= 3
					&& value->second[0] == '@'
					&& value->second[1] == '!')
				{
					// check for escape
					if (value->second[2] == ':') {
						result.append("@!");
						result.append(value->second.substr(3));
					}
					else if (dict) {
						// query requested path
						std::string text = dict->req(value->second.substr(2)).get({});
						result.append(text);
					}
					else {
						// pass through raw request
						result.append(value->second);
					};
				}
				else {
					// add parameter value
					result.append(value->second);
				};
			};
		};

		// copy leftover data
		return result + format.substr(idx);
	};

	/// Prints the path to a stream.
	std::string Path::string(size_t last_idx) const {
		std::string text;

		// replacement string
		const char* repl = "?";

		// print path
		for (size_t i = 0; i <= last_idx; i++) {
			if (i) text.push_back('.');
			if (i < sub.size())
				text.append(sub[i].empty() ? repl : sub[i]);
			else {
				text.append(key.empty() ? repl : key);
				break;
			};
		};
		return text;
	};

	/// Print section data.
	void Section::print(FILE* stream, size_t tabs) const {
		// print opening bracket
		fprintf(stream, "{\n");

		// print each entry
		for (const auto& item : items) {
			// print tabs
			for (size_t i = 0; i < tabs + 1; i++)
				fprintf(stream, "  ");

			// print entry
			fprintf(stream, "%s ", item.key.c_str());
			if (const auto* text = std::get_if<std::unique_ptr<Text>>(&item.value)) {
				// print text
				fprintf(stream, ": \"%s\"", text->get()->format.c_str());
			};
			if (const auto* section = std::get_if<std::unique_ptr<Section>>(&item.value)) {
				// print section
				section->get()->print(stream, tabs + 1);
			};
			fprintf(stream, "\n");
		};

		// print closing bracket
		for (size_t i = 0; i < tabs; i++)
			fprintf(stream, "  ");
		fprintf(stream, "}");
	};
};