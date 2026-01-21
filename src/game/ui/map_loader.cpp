#include "game/ui/map_loader.hpp"
#include "game/game.hpp"
#include "game/values/interface.hpp"

namespace fs = std::filesystem;

namespace gameui {
	/// Loader text field texture.
	static const ui::Panel::Map field_texture = ui::Panel::Map::rect
		(&assets::interface, Values::coords(0, 0), { 6, 6 }, 1);

	/// Construct a map loader.
	Loader::Loader(Game* game, Table* table):
		ui::Panel(field_texture), _game(game), _table(table),
		_select_sec(nullptr), _select_temp(nullptr)
	{
		// configure panel
		infinite = true;
		bounds = { 0.5as, 0as, 660px, 1ps };
		padding.setHorizontal(20);
		color = sf::Color(128, 128, 128);

		ui::Dim width = 300px;
		ui::Dim pad = 20px;

		// create map search
		_search = new ui::TextField(field_texture, Values::panel_text, sf::Color::White);
		_search->bounds = { {}, { width, 56px } };
		_search->padding.setHorizontal(20);
		_search->input.attachTextConfirm([=](const sf::String&) { _search->focus(false); });
		add(_search);

		// filter count label
		_filter = new ui::Text(Values::panel_text, "edit.filter");
		_filter->bounds = { 0px, _search->size().y - 4px, width, 48px };
		_filter->pos = ui::Text::Static;
		_filter->align = ui::Text::C;
		_filter->paramHook("n", [=]() { return ext::str_int(_count); });
		_filter->paramHook("max", [=]() { return ext::str_int(loader.list.size()); });
		add(_filter);

		// load button
		auto* button_cont = new ui::Element;
		button_cont->bounds = { _filter->bounds.bottomLeft(), { width, 64px } };
		button_cont->padding.set(4);
		{
			_load_btn = new ui::Button(field_texture, { 1ps, 1ps });
			_load_btn->position() = { 0.5as, 0.5as };
			_load_btn->exp_coef = 17.f / 16;
			{
				// add button label
				_load_txt = _load_btn->setLabel(Values::panel_text);
				_load_txt->align = ui::Text::S;
				_load_txt->size().y = 0.7ps;
			};
			_load_btn->attach([=]() {
				// ignore if no template selected
				if (!_select_temp) return;

				// construct the map
				construct();

				// deselect the map
				select(nullptr, nullptr);
			});
			button_cont->add(_load_btn);
		};
		add(button_cont);

		// create map list
		_list = new dev::Panel(width);
		_list->scissor = true;
		_list->position() = { 0px, 1as };
		_list->size().y = 1ps - button_cont->bounds.bottomLeft().y;
		add(_list);

		// separator panel
		auto* sep = new ui::Solid;
		sep->bounds = { width + pad / 2.f - 0.5ts, 0px, 6px, 1ps };
		sep->color = sf::Color(16, 16, 16);
		add(sep);

		// map info panel
		auto* info_panel = new ui::Element;
		info_panel->bounds = { width + pad, 0px, width, 1ps };
		{
			ui::Dim height = 48px;
			ui::Dim y = 0px;

			// layout an element
			auto layout = [&](ui::Element* elem) {
				elem->bounds = { 0px, y, 1ps, height };
				info_panel->add(elem);
				y += height + 4px;
			};

			// save button
			auto* save_cont = new ui::Element;
			save_cont->bounds = { 0px, 0px, 1ps, 64px };
			save_cont->padding.set(4);
			y = 64px + 4px;
			{
				auto* button = new ui::Button(field_texture, { 1ps, 1ps });
				button->position() = { 0.5as, 0.5as };
				button->exp_coef = 17.f / 16;
				button->shake_coef = 1.f / 64;
				{
					auto* text = button->setLabel(Values::chat_text);
					text->setPath("edit.save");
				};
				button->validate([=]() {
					// generate template
					auto temp = Template::generate(&_game->map);
					temp.header = {
						.name = _f_name->get(),
						.auth = _f_auth->get()
					};
					::Loader::save(::Loader::File{ _f_file->get(), temp });

					// reload map list
					reload();
					return true;
				});
				save_cont->add(button);
			};
			info_panel->add(save_cont);

			// map info label
			auto* label0 = new ui::Text(Values::chat_text, "edit.map.label");
			{
				label0->pos = ui::Text::Static;
				label0->align = ui::Text::C;
			};

			// header fields
			_f_file = new Field(Values::chat_text, "edit.map.file.k");
			_f_file->limit(16);
			_f_name = new Field(Values::chat_text, "edit.map.name.k");
			_f_name->limit(16);
			_f_auth = new Field(Values::chat_text, "edit.map.auth.k");
			_f_auth->limit(16);

			// size info label
			auto* label1 = new ui::Text(Values::chat_text, "edit.content.label_size");
			{
				label1->pos = ui::Text::Static;
				label1->align = ui::Text::C;
			};

			// size fields
			_f_size = new Field(Values::chat_text, "edit.content.size");
			_f_re = new Field(Values::chat_text, "edit.content.resize");
			_f_re->field().attachValidation([=](const sf::String& str, char32_t c) {
				// accept backspace
				if (c == '\b') return true;

				// accept space if hasn't appeared before
				std::string text = _f_re->get();
				if (c == ' ')
					return text.find(' ') == std::string::npos;

				// accept only numbers
				return c >= '0' && c <= '9';
			});
			_f_re->field().attachTextConfirm([=](const sf::String&) {
				std::string text = _f_re->get();

				// get space position
				size_t space = text.find(' ');
				if (space != std::string::npos) {
					// ignore if one of numbers is empty
					if (space > 0 && space + 1 < text.size()) {
						int w = std::stoi(text.substr(0, space));
						int h = std::stoi(text.substr(space + 1));

						// resize the map
						_game->map.resize({ {}, { w, h } });
					};
				};

				// unfocus & reset text field
				_f_re->field().clear();
				_f_re->unfocus();
			});

			// content info label
			auto* label2 = new ui::Text(Values::chat_text, "edit.content.label_stats");
			{
				label2->pos = ui::Text::Static;
				label2->align = ui::Text::C;
			};

			// content fields
			_f_troop = new Field(Values::chat_text, "edit.content.troop");
			_f_build = new Field(Values::chat_text, "edit.content.build");
			_f_plant = new Field(Values::chat_text, "edit.content.plant");
			_f_reg   = new Field(Values::chat_text, "edit.content.region");
		
			// add unfocus callbacks
			std::vector<Field*> field_list = {
				_f_file, _f_name, _f_auth,
				_f_size, _f_re, _f_troop, _f_build, _f_plant, _f_reg
			};
			bool counters = false;
			for (Field* field : field_list) {
				field->unfocus(field_list);
				if (field == _f_troop) counters = true;

				// custom splits
				if (counters) field->split(0.6f, 0.4f);
				else field->split(0.4f, 0.6f);
			};

			// layout elements
			ui::Element* list[] = {
				label0, _f_file, _f_name, _f_auth,
				label1, _f_size, _f_re,
				label2, _f_troop, _f_build, _f_plant, _f_reg
			};
			for (Element* field : list) layout(field);

			// disable stats fields
			_f_size->disabled = true;
			_f_troop->disabled = true;
			_f_build->disabled = true;
			_f_plant->disabled = true;
			_f_reg->disabled = true;

			// add data sync to fields
			info_panel->onUpdate([=](const sf::Time&) {
				// update size
				_f_size->set(ext::str_vec(_game->map.size(), " x "));

				// update counters
				_f_troop->set(ext::str_int(_game->map._troops.count()));
				_f_build->set(ext::str_int(_game->map._builds.count()));
				_f_plant->set(ext::str_int(_game->map._plants.count()));
				_f_reg->set(ext::str_int(_game->map.regions._pool.count()));
				info_panel->recalculate();
			});

			// clear button
			auto* clear_cont = new ui::Element;
			clear_cont->bounds = { 0px, y, 1ps, 64px };
			clear_cont->padding.set(4);
			{
				auto* button = new ui::Button(field_texture, { 1ps, 1ps });
				button->position() = { 0.5as, 0.5as };
				button->exp_coef = 17.f / 16;
				button->shake_coef = 1.f / 64;
				{
					auto* text = button->setLabel(Values::chat_text);
					text->setPath("edit.clear");
				};
				button->validate([=]() {
					// clear map
					_game->map.clear();
					_game->centerCamera();

					// deselect region resource table
					_table->sync(nullptr);

					// reset file field
					_f_file->set("");
					return true;
				});
				clear_cont->add(button);
			};
			info_panel->add(clear_cont);
		};
		add(info_panel);

		// mask all events
		// @note this will not mask resource table but idrc
		onEvent([](const ui::Event& evt) { return true; });

		// default configuration
		reload();
	};

	/// Constructs a map from template.
	void Loader::construct() {
		// deselect everything
		_game->deselectRegion();
		_game->deselectTile();
		
		// construct the map
		_select_temp->construct(&_game->map);
		_game->centerCamera();

		// deselect region resource table
		_table->sync(nullptr);

		// store generic info in fields
		_f_file->set(_select_file);
		_f_name->set(_select_temp->header.name);
		_f_auth->set(_select_temp->header.auth);
	};

	/// Selects a map from the list.
	void Loader::select(dev::Section* section, const Template* temp) {
		_select_sec = section;
		_select_temp = temp;

		// deselection
		if (!section) {
			// disable button
			_load_btn->disable();

			// change button label
			_load_txt->setPath("edit.load_none");
		}
		else {
			// enable button
			_load_btn->enable();

			// change button label
			_load_txt->setPath("edit.load_file");
			_load_txt->param("file", temp->header.name);
		};
		_load_btn->recalculate();
	};

	/// Reloads map list.
	void Loader::reload() {
		// clear list
		_list->clear();
		select(nullptr, nullptr);

		// load maps
		loader.reload();

		// display maps
		for (const auto& file : loader.list) {
			// add map section
			auto* sec = _list->push([=]() {
				// show if filename contains substring
				bool _ = file.name.find(_search->input.get()) != std::string::npos;
				if (_) _count++;
				return _;
			});
			
			// add map info
			sec->line("edit.map.file.k");
			sec->extra("edit.map.file.v", 0.4ps, sf::Color::Magenta);
			sec->line("edit.map.name.k");
			sec->extra("edit.map.name.v", 0.4ps);
			sec->line("edit.map.auth.k");
			sec->extra("edit.map.auth.v", 0.4ps, sf::Color::Yellow);

			// set arguments
			sec->args = {
				{ "file", file.name },
				{ "name", file.temp.header.name },
				{ "auth", file.temp.header.auth }
			};

			// add selection callback
			const Template* temp = &file.temp;
			sec->onEvent([=](const ui::Event& evt) {
				if (auto data = evt.get<ui::Event::MousePress>()) {
					if (data->button == sf::Mouse::Button::Left) {
						// select the map
						select(sec, temp);
						_select_file = file.name;

						// start button animation
						_load_btn->push(_load_btn->chain(
							_load_btn->emitExpand(),
							_load_btn->emitShrink()
						));
						return true;
					};
				};
				return false;
			});
		};
		_list->recalculate();

		// add counter reset
		_list->attach([=]() { _count = 0; });

		// add map deselection
		_list->onEvent([=](const ui::Event& evt) {
			if (auto data = evt.get<ui::Event::MousePress>()) {
				if (data->button == sf::Mouse::Button::Left) {
					select(nullptr, nullptr);
					return true;
				};
			};
			return false;
		});
	};

	/// Whether any input field is active.
	bool Loader::input() const {
		return _f_file->input() || _f_name->input() || _f_auth->input() || _f_re->input();
	};

	/// Constructs a grabber button.
	Grabber::Grabber(Loader* loader) : ui::Button(field_texture, { 64px, 48px }) {
		// add icon
		setIcon(&assets::interface, Values::grab_icon);

		// put loader in a hidden position
		loader->position() = { 0.5as, -1ps };
		loader->recalculate();

		// attach below the loader
		onRecalculate([=](const sf::Time&) {
			position() = { 0.5as, loader->bounds.bottomLeft().y };

			// disable loader on the first frame
			if (_first) {
				_first = false;
				loader->deactivate(true);
			};
		});

		// add click event
		attach([=]() {
			// enable loader
			loader->activate(true);

			// start animation
			auto* anim = ui::AnimDim::to(&loader->position().y, 0ps, sf::seconds(0.33f));
			anim->ease = ui::Easings::sineOut;
			push(anim);

			// disable grabber
			disable();
		});

		// don't propagate mouse events
		onEvent([=](const ui::Event& evt) {
			if (auto data = evt.get<ui::Event::MousePress>()) {
				return true;
			};
			return false;
		});

		// add loader hiding callback
		loader->onEvent([=](const ui::Event& evt) {
			if (auto data = evt.get<ui::Event::MousePress>()) {
				if (data->button == sf::Mouse::Button::Left) {
					if (!loader->rect().contains(data->position)) {
						// start animation
						auto* anim = ui::AnimDim::to(&loader->position().y, -1ps, sf::seconds(0.33f));
						anim->ease = ui::Easings::sineIn;
						anim->setAfter([=]() { loader->deactivate(true); });
						push(anim);

						// enable grabber
						enable();
					};
				};
			};
			return false;
		});
	};
};