#include "ui/text.hpp"
#include "assetload.hpp"

namespace ui {
	/// Returns alignment multiplier.
	sf::Vector2f Text::alignMultipliers(Align align) {
		// axis multiplier look-up table
		const float lut[4]{ 0.5f, 0.5f, 0.0f, 1.0f };

		// map alignment axes into their values
		return { lut[align & 3], lut[align >> 2 & 3] };
	};

	/// Sets text positioning mode.
	void Text::setPositioning(int pos) {
		this->pos = static_cast<Positioning>(pos);
	};
	/// Sets text alignment.
	void Text::setAlign(int align) {
		this->align = static_cast<Align>(align);
	};

	/// Recalculates text state.
	void Text::recalc() {
		if (!_raw) {
			List* list = _shargs;

			// own argument list
			if (!list) {
				list = &_args;

				// evaluate auto-loading arguments
				for (const auto& gen : _autoargs) {
					if (auto value = gen.second())
						_args[gen.first] = *value;
				};
				for (const auto& gen : _autovargs) gen();
			};

			// evaluate text parameters
			std::string value = _format.get(*list, &assets::lang::locale);

			// set new value
			_text.setString(value);
		};

		// set label size
		if (autosize) bounds.size = _text.getLocalBounds().size;
	};

	/// Reloads text.
	void Text::onTranslate() {
		_format = _path.empty ? localization::Text() : assets::lang::locale.req(_path);
	};

	/// Draws the label.
	void Text::drawSelf(RenderBuffer& target, sf::IntRect self) const {
		// reconfigure text object
		auto rect = _text.getLocalBounds();
		if (pos & 1) {
			// shrink horizontally
			self.position.x -= (int)rect.position.x;
		};
		if (pos & 2) {
			// shrink vertically
			self.position.y -= (int)rect.position.y;
		};
		if (pos & 4) {
			// set height to line spacing
			rect.size.y = _text.getFont().getLineSpacing(_text.getCharacterSize());
		};
		sf::Vector2f mults = alignMultipliers(align);
		_text.setPosition({
			self.position.x + (self.size.x - rect.size.x) * mults.x,
			self.position.y + (self.size.y - rect.size.y) * mults.y,
		});

		// render text
		target.text(_text);
		target.forward(nullptr);
	};

	/// Constructs a text element.
	Text::Text(const TextSettings& settings, const localization::Path& path = {})
		: _text({ settings.font, "", settings.size }), _path(path), _raw(false), _shargs(nullptr)
	{
		// adds recalculation update
		onRecalculate([=](const sf::Time& _) { recalc(); });

		// load text format
		onTranslate();
		setColor(settings.fill);
		setOutline(settings.outline, settings.thickness);
	};

	/// Constructs a text element from raw string.
	Text* Text::raw(const TextSettings& settings, const sf::String& string) {
		Text* text = new Text(settings, "");
		text->setRaw(string);
		return text;
	};

	/// Sets text label to a raw string.
	void Text::setRaw(const sf::String& string) {
		_text.setString(string);
		_raw = true;
	};
	/// Sets text label to a localization path.
	void Text::setPath(const localization::Path& path) {
		_path = path;
		_raw = false;
		onTranslate();
		recalc();
	};

	/// @return Current text label.
	const sf::String& Text::string() const {
		return _text.getString();
	};
	/// Returns character position in current text label.
	sf::Vector2f Text::charpos(size_t idx) const {
		return _text.getInverseTransform().transformPoint(_text.findCharacterPos(idx));
	};

	/// Uses an external argument list.
	void Text::use(List* list) {
		_shargs = list;
	};

	/// Clears text arguments.
	void Text::paramClear() {
		_args.clear();
	};
	/// Clears text argument hooks.
	void Text::paramHookClear() {
		_autoargs.clear();
	};
	/// Clears argument evaluation hooks.
	void Text::hookClear() {
		_autovargs.clear();
	};

	/// Sets format argument value.
	void Text::param(std::string name, std::string value) {
		_args[name] = value;
	};
	/// Adds a format argument generator hook.
	void Text::paramHook(std::string name, std::function<Hook()> generator) {
		_autoargs[name] = generator;
	};
	/// Adds an argument evaluation callback.
	void Text::hook(Element::StaticHandler call) {
		_autovargs.push_back(call);
	};

	/// Configures text character size.
	void Text::setSize(unsigned int size) const {
		_text.setCharacterSize(size);
	};
	/// Configures text scaling.
	void Text::setScale(float scale) const {
		_text.setScale({ scale, scale });
	};

	/// Configures text color.
	void Text::setColor(sf::Color color) const {
		_text.setFillColor(color);
	};

	/// Configures text outline.
	void Text::setOutline(sf::Color color, float thickness) const {
		_text.setOutlineColor(color);
		_text.setOutlineThickness(thickness);
	};
};