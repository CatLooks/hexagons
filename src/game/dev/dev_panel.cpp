#include "dev/dev_panel.hpp"

namespace dev {
	/// Panel text settings.
	const ui::TextSettings Panel::settings = {
		assets::font, 16, sf::Color::White, sf::Color::Black, 2.f
	};

	/// Panel text height.
	const float Panel::height = 24;

	/// Constructs a section element.
	Section::Section() {
		// set panel color
		color = sf::Color(0, 0, 0, 64);
		padding.set(4);
		bounds = { 0px, 0px, 1ps, (float)(padding.top + padding.bottom) };
	};

	/// Pushes a new text line.
	ui::Text* Section::line(const localization::Path& path, sf::Color color) {
		// create the label
		ui::Text* text = new ui::Text(Panel::settings, path);

		// configure text label
		text->align = ui::Text::NW;
		text->pos = ui::Text::Static;
		text->bounds = { 0px, height(), 1ps, Panel::height };
		text->use(&args);
		text->setColor(color);

		// register text element
		lines.push_back(text);
		add(text);

		// update section size
		size().y += Panel::height;
		return text;
	};

	/// Pushes a new text line.
	ui::Text* Section::extra(const localization::Path& path, sf::Color color) {
		// create the label
		ui::Text* text = new ui::Text(Panel::settings, path);

		// configure text label
		text->align = ui::Text::NW;
		text->pos = ui::Text::Static;
		text->bounds = { 0.5ps, height() - Panel::height, 1ps, Panel::height };
		text->use(&args);
		text->setColor(color);

		// register text element
		lines.push_back(text);
		add(text);
		return text;
	};

	/// Returns section height.
	float Section::height() const {
		return lines.size() * Panel::height;
	};

	/// Constructs the developer panel.
	Panel::Panel(ui::Dim width) {
		// configure panel
		bounds = { 0px, 0px, width, 1ps };
		color = sf::Color(0, 0, 0, 64);
		padding.set(8);
	};

	/// Pushes a new section to the panel.
	Section* Panel::push(std::function<bool()> predicate) {;
		Section* sec = new Section;
		_ctx.push_back({ sec, predicate });
		add(sec);
		return sec;
	};

	/// Draws text sections.
	void Panel::drawChildren(ui::RenderBuffer& buffer) const {
		ui::Dim y = 0px;

		// draw all active sections
		for (const auto& info : _ctx) {
			// ignore if predicate is not met
			if (!info.pred()) continue;

			// set section position
			info.elem->position().y = y;
			info.elem->recalculate();
			y += info.elem->height() + height;

			// draw section
			info.elem->draw(buffer);
		};
	};
};