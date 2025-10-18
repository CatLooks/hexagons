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
		std::function<localization::Text()> _reload; /// Format reloader.
		localization::Text _format;                  /// Format object.
		std::unique_ptr<sf::Text> _text;             /// Rendering object.

		/// Recalculates text state.
		void onRecalculate() override;
		/// Reloads text.
		void onTranslate() override;

		/// Draws the label.
		void drawSelf(RenderBuffer& target, sf::IntRect self) const override;

	public:
		/// Whether to automatically update label size to text size.
		bool autosize = false;

		/// Constructs a text element.
		/// 
		/// @param settings Text settings.
		/// @param path Text localization path.
		Text(const TextSettings& settings, const localization::Path& path);

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