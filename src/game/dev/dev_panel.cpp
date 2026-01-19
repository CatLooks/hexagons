#include "dev/dev_panel.hpp"

namespace dev {
	/// Panel text settings.
	const ui::TextSettings Section::settings = {
		assets::font, 16, sf::Color::White, sf::Color::Black, 2.f
	};

	/// Panel text height.
	const float Panel::height = 22.f;

	/// Constructs a section element.
	Section::Section(const ui::TextSettings& sets) : h(0.f), sets(sets) {
		// set panel color
		color = sf::Color(0, 0, 0, 64);
		padding.setHorizontal(8);
		padding.setVertical(4);
		bounds = { 0px, 0px, 1ps, (float)(padding.top + padding.bottom) };
	};

	/// Clears the section.
	void Section::clear() {
		// clear all lines
		lines.clear();
		Solid::clear();

		// reset height
		bounds.size.y = (float)(padding.top + padding.bottom);
		h = 0.f;
	};

	/// Pushes a new text line.
	ui::Text* Section::line(const localization::Path& path, sf::Color color) {
		// create the label
		ui::Text* text = new ui::Text(sets, path);

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
		h += Panel::height;
		return text;
	};

	/// Pushes a new text line.
	ui::Text* Section::extra(const localization::Path& path, ui::Dim offset, sf::Color color) {
		// create the label
		ui::Text* text = new ui::Text(sets, path);

		// configure text label
		text->align = ui::Text::NW;
		text->pos = ui::Text::Static;
		text->bounds = { offset, height() - Panel::height, 1ps, Panel::height };
		text->use(&args);
		text->setColor(color);

		// register text element
		lines.push_back(text);
		add(text);
		return text;
	};

	/// Returns section height.
	float Section::height() const {
		return h;
	};

	/// Attaches an argument update callback.
	void Section::attach(StaticHandler handler) {
		argupdate = handler;
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

	/// Attaches a global update callback.
	void Panel::attach(StaticHandler handler) {
		_upd = handler;
	};

	/// Draws text sections.
	void Panel::drawChildren(ui::RenderBuffer& buffer) const {
		ui::Dim y = 0px;

		// global update
		if (_upd) _upd();

		// draw all active sections
		for (const auto& info : _ctx) {
			// ignore if predicate is not met
			if (!info.pred()) continue;

			// update arguments
			if (info.elem->argupdate)
				info.elem->argupdate();

			// set section position
			info.elem->position().y = y;
			info.elem->recalculate();
			y += info.elem->height() + height * 0.5f;

			// draw section
			info.elem->draw(buffer);
		};
	};

	/// Constructs a panel using the layout.
	void SectionLayout::construct(Section* section) const {
		// add title
		section->line(title, sf::Color::Magenta);
		
		// add key-value pairs
		for (const auto& p : kv) {
			section->line(p + ".k", sf::Color::White);
			section->extra(p + ".v", 0.5ps, sf::Color::White);
		};
	};
};