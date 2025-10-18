#pragma once

// include dependencies
#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <memory>
#include <map>
#include <initializer_list>

namespace localization {
	// forward declarations
	struct Text;
	struct Section;

	/// Variant value type.
	using Value = std::variant<
		std::unique_ptr<Text>,
		std::unique_ptr<Section>
	>;

	/// Section access key path.
	struct Path {
		std::vector<std::string> sub; /// Path recursive accesses.
		std::string key;              /// Final path access.
		bool local = false;           /// Whether the path is local.

		/// Constructs an empty path.
		Path();
		/// Constructs a path from a C-string.
		Path(const char* string);

		/// Converts the path to a string.
		/// 
		/// @param last_idx Last index to display.
		std::string string(size_t last_idx = ~0ull) const;
	};
	/// Text parameter instruction.
	struct Param {
		size_t pos;      /// Position in format string.
		std::string key; /// Parameter key.
	};

	/// Localized text value.
	struct Text {
		/// Argument specifier structure.
		struct Arg {
			std::string name;  /// Argument name.
			std::string value; /// Argument value.
		};

		std::string format;        /// Format string.
		std::vector<Param> params; /// Parameter list.

		/// Creates a formatted text string.
		/// 
		/// If an argument value cannot be found, it will be replaced by `{name}` string.
		/// 
		/// Extra argument names are ignored.
		/// 
		/// @param args Format arguments.
		/// 
		/// @return Formatted string.
		std::string get(std::initializer_list<Arg> args = {}) const;

		template <typename... Args>
		std::string get(Args&&... args) const {
			return get({ Arg(args)... });
		};
	};

	/// Section entry data.
	struct Entry {
		std::string key; /// Entry key.
		Value value;     /// Entry value.

		/// Constructs a new entry.
		///
		/// @param key Entry key.
		/// @param value Entry text value (owning pointer).
		Entry(std::string key, Text* value);
		/// Constructs a new entry.
		///
		/// @param key Entry key.
		/// @param value Entry section value (owning pointer).
		Entry(std::string key, Section* value);
		/// Constructs a new entry.
		///
		/// @param key Entry key.
		/// @param value Entry text value.
		Entry(std::string key, Text value);
		/// Constructs a new entry.
		///
		/// @param key Entry key.
		/// @param value Entry section value.
		Entry(std::string key, Section value);
	};

	/// Data section value.
	struct Section {
		std::vector<Entry> items; /// Section entries.

		/// Returns a value denoted by a key.
		///
		/// @param key Entry key.
		///
		/// @return Value or `nullptr` if a key is not found.
		const Value* get(std::string key) const;

		/// Returns a value denoted by a recursive path.
		///
		/// @param path Value path.
		///
		/// @return Value pointer or index of a bad access.
		std::variant<const Value*, size_t> get(Path path) const;

		/// Requests the text value at a path.
		/// 
		/// @param path Requested path.
		/// 
		/// @return Text value or a replacement string if path is invalid.
		Text req(const Path& path) const;

		/// Print section data.
		///
		/// @param stream Output stream.
		/// @param tabs Text tabbing.
		void print(FILE* stream, size_t tabs = 0) const;
	};
};