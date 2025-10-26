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
	};

	/// Text label element.
	class Text : public Element {
	public:
		/// Alignment type.
		enum Align {
			Center = 0, /// Align to center.

			Left   = 0b0010, /// Align to left center.
			Right  = 0b0011, /// Align to right center.
			Top    = 0b1000, /// Align to top center.
			Bottom = 0b1100, /// Align to bottom center.

			TopLeft     = Top    | Left , /// Align to top left corner.
			TopRight    = Top    | Right, /// Align to top right corner.
			BottomLeft  = Bottom | Left , /// Align to bottom left corner.
			BottomRight = Bottom | Right, /// Align to bottom right corner.

			C = Center, /// Align to center.

			W = Left,   /// Align to west (same as `Left`).
			E = Right,  /// Align to east (same as `Right`).
			N = Top,    /// Align to north (same as `Top`).
			S = Bottom, /// Align to south (same as `Bottom`).

			NW = N | W, /// Align to north west (same as `TopLeft`).
			NE = N | E, /// Align to north east (same as `TopRight`).
			SW = S | W, /// Align to south west (same as `BottomLeft`).
			SE = S | E, /// Align to south east (same as `BottomRight`).
		};
		
		/// Text alignment within the label bounding box.
		Align align = NW;

		/// Returns alignment axis multipliers.
		static sf::Vector2f alignMultipliers(Align align);

		/// Automatic argument generator return type.
		using Hook = std::optional<std::string>;

	protected:
		/// Text localization path.
		localization::Path _path;
		/// Text format object.
		localization::Text _format;
		/// Text rendering object.
		std::unique_ptr<sf::Text> _text;
		/// Format arguments.
		std::unordered_map<std::string, std::string> _args;
		/// Automatic argument setters.
		std::unordered_map<std::string, std::function<Hook()>> _autoargs;
		/// Automatic multi-argument setters.
		std::list<Element::StaticHandler> _autovargs;

		/// Recalculates text state.
		void recalc();
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
		/// Clears argument evaluation hooks.
		void hookClear();

		/// Sets format argument value.
		/// 
		/// @param name Argument name.
		/// @param value Argument value.
		void param(std::string name, std::string value);
		/// Adds a format argument generator hook.
		/// 
		/// @param name Argument name.
		/// @param generator Argument generator.
		void paramHook(std::string name, std::function<Hook()> generator);
		/// Adds an argument evaluation callback.
		/// 
		/// @param call Callback function.
		void hook(Element::StaticHandler call);

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