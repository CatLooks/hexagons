#pragma once

// include dependencies
#include <vector>
#include <string>
#include <variant>
#include <memory>

namespace localization {
	/// Localized text value.
	struct Text;
	/// Data section value.
	struct Section;

	/// Text import instruction.
	struct Import {
		size_t offset;   /// Offset since last instruction.
		std::string req; /// Request string.
	};
	/// Text parameter instruction.
	struct Param {
		size_t offset;   /// Offset since last instruction.
		std::string key; /// Parameter key.
	};

	/// Localized text value.
	struct Text {
		std::string format;        /// Format string.
		std::vector<Param> params; /// Parameter list.
	};
	/// Raw localized string data.
	struct RawText {
		std::string format;          /// Format string.
		std::vector<Import> imports; /// Import list.
		std::vector<Param> params;   /// Parameter list.
	};

	/// Section access key path.
	struct Path {
		std::vector<std::string> sub; /// Path recursive accesses.
		std::string key;              /// Final path access.
	};
	/// Section entry data.
	class Entry {
	public:
		/// Entry value type.
		enum Type {
			Text,
			Section
		};

	private:
		/// Entry value.
		void* _value;
		Type _type;

	public:
		/// Entry access key.
		std::string key;

		/// Constructs an entry with a text value.
		/// 
		/// @param key Entry key.
		/// @param text Entry value.
		Entry(std::string key, localization::Text text);
		/// Constructs an entry with a section value.
		/// 
		/// @param key Entry key.
		/// @param section Entry value.
		Entry(std::string key, localization::Section section);

		/// Destructor.
		~Entry();

		/// @return Pointer to stored text if present.
		localization::Text* text() const;
		/// @return Pointer to stored section if present.
		localization::Section* section() const;
		/// @return Entry value type.
		Type type() const;
	};
	/// Data section value.
	struct Section {
		std::vector<Entry> items; /// Section entries.
	};
};