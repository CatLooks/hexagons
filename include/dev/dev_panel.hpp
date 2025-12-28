#pragma once

// include dependencies
#include "ui.hpp"
#include "assets.hpp"

/// Developer stuff.
namespace dev {
	class Panel;

	/// Text section element.
	class Section : public ui::Solid {
		friend Panel;

	private:
		/// Text lines.
		std::vector<ui::Text*> lines;
		/// Argument update function.
		StaticHandler argupdate;
		/// Height.
		float h;

	public:
		/// Shared text argument list.
		ui::Text::List args;

		/// Constructs a section element.
		Section();

		/// Text line position.
		enum Position { Left, Right };

		/// Adds a text line to the section.
		/// 
		/// @param path Text label localization path.
		/// @param color Text color (white by default).
		/// 
		/// @return Text element reference.
		ui::Text* line(const localization::Path& path, sf::Color color = sf::Color::White);

		/// Adds an extra text line to the last normal text line.
		/// 
		/// @param path Text label localization path.
		/// @param color Text color (white by default).
		/// 
		/// @return Text element reference.
		ui::Text* extra(const localization::Path& path, sf::Color color = sf::Color::White);

		/// Returns section height.
		float height() const;

		/// Attaches an argument update callback.
		/// 
		/// @param handler Callback function.
		void attach(StaticHandler handler);
	};

	/// Developer panel.
	class Panel : public ui::Solid {
	public:
		/// Panel text settings.
		static const ui::TextSettings settings;
		/// Panel text height.
		static const float height;

	private:
		/// Section info.
		struct secinfo {
			/// Section element pointer.
			Section* elem;
			/// Section display predicate.
			std::function<bool()> pred;
		};

		/// List of sections.
		std::vector<secinfo> _ctx;

	public:
		/// Constructs the developer panel.
		/// 
		/// @param width Panel width.
		Panel(ui::Dim width);

		/// Pushes a new section to the panel.
		///
		/// @param predicate When to display the section (always by default).
		/// 
		/// @return New section element.
		Section* push(std::function<bool()> predicate = []() { return true; });

	protected:
		/// Draws text sections.
		void drawChildren(ui::RenderBuffer& buffer) const override;
	};

	/// Section layout description.
	struct SectionLayout {
		/// Section title.
		std::string title;
		/// Section key-value pairs.
		///
		/// Suffixed with `.k` and `.v`.
		std::vector<std::string> kv;

		/// Constructs a panel using the layout.
		/// 
		/// @param section Target section.
		void construct(Section* section) const;
	};
};