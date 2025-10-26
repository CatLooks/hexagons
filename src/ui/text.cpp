#include "ui/text.hpp"
#include "assetload.hpp"

namespace ui {
	/// Returns alignment multiplier.
	sf::Vector2f Text::alignMultipliers(Align align) {
		// axis multiplier look-up table
		const float lut[4] { 0.5f, 0.5f, 0.0f, 1.0f };

		// map alignment axes into their values
		return { lut[align & 3], lut[align >> 2 & 3] };
	};

	/// Recalculates text state.
	void Text::recalc() {
		// evaluate auto-loading arguments
		for (const auto& gen : _autoargs) {
			if (auto value = gen.second())
				_args[gen.first] = *value;
		};
		for (const auto& gen : _autovargs) gen();

		// evaluate text parameters
		std::string value = _format.get(_args);

		// set new value
		_text.get()->setString(value);

		// set label size
		if (autosize) bounds.size = _text.get()->getLocalBounds().size;
	};

	/// Reloads text.
	void Text::onTranslate() {
		_format = assets::lang::locale.req(_path);
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
		sf::Vector2f mults = alignMultipliers(align);
		text->setPosition({
			self.position.x + (self.size.x - rect.size.x) * mults.x - rect.position.x,
			self.position.y + (self.size.y - rect.size.y) * mults.y - rect.position.y,
		});

		// render text
		target.text(*text);
	};

	/// Constructs a text element.
	Text::Text(const TextSettings& settings, const localization::Path& path):
		_text(new sf::Text(settings.font, "", settings.size)),
		_path(path),
		_format(assets::lang::locale.req(path))
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