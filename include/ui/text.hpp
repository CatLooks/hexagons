#pragma once

// include dependencies
#include "element.hpp"
#include "localization/token.hpp"

namespace ui {
	/// Collection of text label construction settings.
	struct TextSettings {
		/// Text font.
		const sf::Font& font;
		/// Text size.
		unsigned int size;
		/// Text locale.
		const localization::Section& locale;
	};

	/// Text label element.
	class Text : public Element {
	public:
		/// Alignment type.
		enum Align {
			Center = 0, /// Center contents.
			Left   = 1, /// Stick contents to left.
			Right  = 2, /// Stick contents to right.
			Up     = 1, /// Stick contents to top.
			Down   = 2, /// Stick contents to bottom.
		};

		Align alignX = Left; /// Alignment on X-axis.
		Align alignY = Left; /// Alignment on Y-axis.

		/// Returns alignment multiplier.
		static float alignMultiplier(Align align);

	protected:
		/// Format reloader.
		std::function<localization::Text()> _reload;
		/// Text format object.
		localization::Text _format;
		/// Text rendering object.
		std::unique_ptr<sf::Text> _text;
		/// Format arguments.
		std::unordered_map<std::string, std::string> _args;
		/// Automatic argument setters.
		std::unordered_map<std::string, std::function<std::string()>> _autoargs;

		/// Recalculates text state.
		void onRecalculate() override;
		/// Reloads text.
		void onTranslate() override;

		/// Draws the label.
		void drawSelf(RenderBuffer& target, sf::IntRect self) const override;

	public:
		/// Whether to automatically set label size to text size.
		bool autosize = false;
		/// Whether to shrink text bounding box optimally.
		/// 
		/// Setting this to `true` may result in text jumps when contents change.
		/// Setting this to `false` may result in text not looking properly vertically aligned.
		bool shrink_to_fit = false;

		/// Constructs a text element.
		/// 
		/// @param settings Text settings.
		/// @param path Text localization path.
		Text(const TextSettings& settings, const localization::Path& path);

		/// Clears text arguments.
		void paramClear();
		/// Clears text argument hooks.
		void paramHookClear();
		/// Sets format argument value.
		/// 
		/// @param name Argument name.
		/// @param value Argument value.
		void param(std::string name, std::string value);
		/// Adds a format argument generator hook.
		/// 
		/// @param name Argument name.
		/// @param generator Argument generator.
		void paramHook(std::string name, std::function<std::string()> generator);

		/// Configures text scaling.
		/// 
		/// @param scale Text scale.
		void setScale(float scale) const;
		/// Configures text color.
		/// 
		/// @param color Text color.
		void setColor(sf::Color color) const;
		/// Configures text outline.
		/// 
		/// @param color Outline color.
		/// @param thickness Outline thickness.
		void setOutline(sf::Color color, float thickness) const;
	};
};