#include "game/ui/resource_table.hpp"

namespace gameui {
	/// Constructs a number field.
	Field::Field(const ui::TextSettings& settings, const localization::Path& label) {
		infinite = true;

		// create label
		_label = new ui::Text(settings, label);
		_label->pos = ui::Text::Static;
		_label->align = ui::Text::C;
		add(_label);

		// create input field
		_field = new ui::TextField({ .texture = nullptr }, settings, sf::Color::White);
		_field->color = Values::dimTint;
		_field->padding.setHorizontal(8);
		add(_field);

		// default split
		split(0.5f, 0.5f);

		// add field disabling routine
		_field->onFocus([=](bool focused) {
			if (disabled && focused) _field->focus(false);
		});
	};

	/// Sets a split between label space and input space.
	void Field::split(float a, float b) {
		_label->bounds = { 0as, 0ps, 1ps * a, 1ps };
		_field->bounds = { 1as, 0px, 1ps * b, 1ps };
	};

	/// Adds an icon to the field.
	ui::Image* Field::icon(float a, float b, const sf::Texture* texture, sf::IntRect coords, float scale) {
		// set split
		split(a, b);

		// create image container
		auto* cont = new ui::Element;
		cont->bounds = { 1ps * a, 0px, 1ps * (1.f - a - b), 1ps};
		add(cont);
		{
			ui::Image* img = new ui::Image(texture, coords);
			img->position() = { 0.5as, 0.5as };
			img->size() *= scale;
			cont->add(img);
			return img;
		};
	};

	/// Makes the field numeric.
	void Field::numeric(size_t digits, std::function<void(int result)> call) {
		_field->input.attachValidation([=](const sf::String& str, char32_t c) {
			// accept backspace
			if (c == '\b') return true;

			// ignore if too much digits
			if (str.getSize() > digits)
				return false;

			// only accept digits
			return c >= '0' && c <= '9';
		});
		_field->input.attachTextConfirm([=](const sf::String& str) {
			// convert string to int
			int result = std::stoi(str.toAnsiString());

			// invoke callback
			call(result);

			// unfocus text field
			_field->focus(false);
		});
	};

	/// Sets input field value.
	void Field::set(const std::string& text) {
		_field->input.set(sf::String::fromUtf8(text.begin(), text.end()));
	};

	/// Whether any input field is active.
	bool Field::input() const {
		return _field->focused();
	};

	/// Constructs the resource table.
	Table::Table() : _reg(nullptr) {
		infinite = true;

		// create fields
		ui::Dim height = 56px;
		_money  = new Field(Values::chat_text, "edit.region.money");
		_peach  = new Field(Values::chat_text, "edit.region.peach");
		_berry  = new Field(Values::chat_text, "edit.region.berry");
		_income = new Field(Values::chat_text, "edit.region.income");

		// configure fields
		struct data {
			Field* field;
			sf::IntRect icon;
		};
		const data list[] = {
			{ _money, Values::coin_icon },
			{ _peach, Values::peach_icon },
			{ _berry, Values::berry_icon },
			{ _income, Values::income_arrow[1] }
		};
		ui::Dim y = 0px;
		for (const auto& e : list) {
			// add icon
			_arrow = e.field->icon(0.4f, 0.4f, &assets::interface, e.icon, 1.f);

			// add field to table
			e.field->bounds = { 0px, y, 1ps, height };
			y += height + 8px;
			add(e.field);
		};
		size().y = y;

		// disable income field
		_income->disabled = true;

		// add setters
		_money->numeric(5, [=](int result) {
			// ignore if no region attached
			if (!_reg) return;

			// set region money
			_reg->money = result;
		});
		_peach->numeric(5, [=](int result) {
			// ignore if no region attached
			if (!_reg) return;

			// set region peaches
			_reg->peach = result;
		});
		_berry->numeric(5, [=](int result) {
			// ignore if no region attached
			if (!_reg) return;

			// set region berries
			_reg->berry = result;
		});

		// add field value updates
		onUpdate([=](const sf::Time&) {
			// update fields
			Field* list[] = {_money, _peach, _berry};
			for (Field* field : list)
				field->disabled = !_reg;

			// empty fields if no region
			if (!_reg) {
				_money->set("-");
				_peach->set("-");
				_berry->set("-");
				_income->set("-");
				_arrow->coords = Values::income_arrow[1];
				return;
			};

			// sync fields to region stats
			if (!_money->input()) _money->set(ext::str_int(_reg->money));
			if (!_peach->input()) _peach->set(ext::str_int(_reg->peach));
			if (!_berry->input()) _berry->set(ext::str_int(_reg->berry));

			// sync income
			_income->set(ext::str_int(_reg->income));
			int color = 1;
			if (_reg->income > 0) color = 0;
			if (_reg->income < 0) color = 2;
			_arrow->coords = Values::income_arrow[color];
		});
	};

	/// Attaches a region to the table.
	void Table::sync(Region* reg) {
		_reg = reg;
	};

	/// Whether any input field is active.
	bool Table::input() const {
		return _money->input() || _peach->input() || _berry->input();
	};
};