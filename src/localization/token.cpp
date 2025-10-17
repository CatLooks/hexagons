#include "localization/token.hpp"

namespace localization {
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
					path.sub.pop_front();

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

	/// Print section data.
	void Section::print(FILE* stream, size_t tabs) const {
		// print opening bracket
		fprintf(stream, "{\n");

		// print each entry
		for (const auto& item : items) {
			// print tabs
			for (size_t i = 0; i < tabs + 1; i++)
				fprintf(stream, "  ", i);

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
			fprintf(stream, "  ", i);
		fprintf(stream, "}");
	};
};