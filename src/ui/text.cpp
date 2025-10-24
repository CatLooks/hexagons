#include "ui/text.hpp"

namespace ui {
	/// Returns alignment multiplier.
	float Text::alignMultiplier(Align align) {
		switch (align) {
			case Left: return 0.f;
			case Right: return 1.f;
			default: return 0.5f;
		};
	};

	/// Recalculates text state.
	void Text::recalc() {
		// evaluate auto-loading arguments
		for (const auto& gen : _autoargs) {
			if (auto value = gen.second())
				_args[gen.first] = *value;
		};

		// evaluate text parameters
		std::string value = _format.get(_args);

		// set new value
		_text.get()->setString(value);

		// set label size
		if (autosize) bounds.size = _text.get()->getLocalBounds().size;
	};

	/// Reloads text.
	void Text::onTranslate() {
		_format = _reload();
	};

	/// Draws the label.
	void Text::drawSelf(RenderBuffer& target, sf::IntRect self) const {
		sf::Text* text = _text.get();

		// reconfigure text object
		auto rect = text->getLocalBounds();
		if (!shrink_to_fit) {
			rect.size += rect.position;
			rect.position = {};
		};
		text->setPosition({
			self.position.x + (self.size.x - rect.size.x) * alignMultiplier(alignX) - rect.position.x,
			self.position.y + (self.size.y - rect.size.y) * alignMultiplier(alignY) - rect.position.y
		});

		// render text
		target.text(*text);
	};

	/// Creates a format string reloader function.
	static std::function<localization::Text()> createReloadFunc(const localization::Section* locale, localization::Path path) {
		return [=]() { return locale->req(path); };
	};

	/// Constructs a text element.
	Text::Text(const TextSettings& settings, const localization::Path& path):
		_text(new sf::Text(settings.font, "", settings.size)),
		_format(settings.locale.req(path)),
		_reload(createReloadFunc(&settings.locale, path))
	{
		// adds recalculation update
		onRecalculate([=](const sf::Time& _) { recalc(); });
	};

	/// Clears text arguments.
	void Text::paramClear() {
		_args.clear();
	};
	/// Clears text argument hooks.
	void Text::paramHookClear() {
		_autoargs.clear();
	};
	/// Sets format argument value.
	void Text::param(std::string name, std::string value) {
		_args[name] = value;
	};
	/// Adds a format argument generator hook.
	void Text::paramHook(std::string name, std::function<Hook()> generator) {
		_autoargs[name] = generator;
	};

	/// Configures text scaling.
	void Text::setScale(float scale) const {
		_text.get()->setScale({ scale, scale });
	};

	/// Configures text color.
	void Text::setColor(sf::Color color) const {
		_text.get()->setFillColor(color);
	};

	/// Configures text outline.
	void Text::setOutline(sf::Color color, float thickness) const {
		_text.get()->setOutlineColor(color);
		_text.get()->setOutlineThickness(thickness);
	};
};