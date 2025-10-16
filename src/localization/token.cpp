#include "localization/token.hpp"

namespace localization {
	/// Constructs an entry with a text value.
	Entry::Entry(std::string key, localization::Text text) : key(key), _type(Text) {
		auto p = new localization::Text;
		*p = text;
		_value = p;
	};
	/// Constructs an entry with a section value.
	Entry::Entry(std::string key, localization::Section section) : key(key), _type(Section) {
		auto p = new localization::Section;
		*p = section;
		_value = p;
	};

	/// Destructor.
	Entry::~Entry() {
		switch (_type) {
			case Text: delete (localization::Text*)_value; break;
			case Section: delete (localization::Section*)_value; break;
		};
	};

	/// @return Pointer to stored text if present.
	Text* Entry::text() const {
		return _type == Text ? (localization::Text*)_value : nullptr;
	};

	/// @return Pointer to stored section if present.
	Section* Entry::section() const {
		return _type == Section ? (localization::Section*)_value : nullptr;
	};

	/// @return Entry value type.
	Entry::Type Entry::type() const {
		return _type;
	};
};