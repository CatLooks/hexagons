#pragma once

// include dependencies
#include <vector>
#include <list>
#include <string>
#include <variant>
#include <optional>
#include <memory>

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
		std::list<std::string> sub; /// Path recursive accesses.
		std::string key;            /// Final path access.
		bool local;                 /// Whether the path is local.
	};
	/// Text import instruction.
	struct Import {
		size_t pos; /// Position in format string.
		Path req;   /// Import path.
	};
	/// Text parameter instruction.
	struct Param {
		size_t pos;      /// Position in format string.
		std::string key; /// Parameter key.
	};

	/// Localized text value.
	struct Text {
		std::string format;        /// Format string.
		std::vector<Param> params; /// Parameter list.
	};
	/// Raw localized string data.
	struct RawText {
		/// Format string.
		std::string format;
		/// Import & parameter list.
		std::vector<std::variant<Import, Param>> params;
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

		/// Print section data.
		///
		/// @param stream Output stream.
		/// @param tabs Text tabbing.
		void print(FILE* stream, size_t tabs = 0) const;
	};
};