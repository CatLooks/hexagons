#include "game/ui/map_loader.hpp"
#include "game/game.hpp"

#include "game/values/interface.hpp"
#include "game/serialize/map.hpp"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace gameui {
	/// Loader text field texture.
	static const ui::Panel::Map field_texture = ui::Panel::Map::rect
		(&assets::interface, Values::coords(0, 0), { 6, 6 }, 1);

	/// Construct a map loader.
	Loader::Loader(Game* game):
		ui::Panel(field_texture), _game(game),
		_select_sec(nullptr), _select_temp(nullptr)
	{
		// set bounds
		bounds = { 0.5as, 0as, 640px, 1ps };
		padding.setHorizontal(20);

		// set color
		color = sf::Color(128, 128, 128);

		// map finder width
		ui::Dim width = 300px;

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
		_filter->paramHook("max", [=]() { return ext::str_int(_temp.size()); });
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
		_list->position() = { 0px, 1as };
		_list->size().y = 1ps - button_cont->bounds.bottomLeft().y;
		add(_list);

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
		fs::path map_folder = "./maps/";

		// clear list
		_list->clear();
		_temp.clear();
		select(nullptr, nullptr);

		// check if folder exists
		if (!fs::exists(map_folder)) {
			fprintf(stderr, "map folder does not exist\n");
			return;
		};

		// check if folder is a directory
		if (!fs::is_directory(map_folder)) {
			fprintf(stderr, "map folder is not a directory\n");
			return;
		};

		// iterate through all files
		for (const auto& file : fs::directory_iterator(map_folder)) {
			// get filename
			std::string name = file.path().filename().generic_string();
			{
				// remove extension
				size_t last = name.find_last_of(".");
				if (last != std::string::npos)
					name = name.substr(0, last);
			};
			fprintf(stderr, "loading %s\n", name.c_str());

			// open file
			std::ifstream str(file.path(), std::ios::binary);
			if (!str) {
				fprintf(stderr, "failed to open file\n");
				continue;
			};

			// read file contents
			std::vector<char> buffer = std::vector<char>(
				std::istreambuf_iterator<char>(str),
				std::istreambuf_iterator<char>()
			);
			fprintf(stderr, "read %zu bytes\n", buffer.size());
			str.close();

			// deserialize file data
			sf::Packet packet;
			packet.append(buffer.data(), buffer.size());
			if (!Serialize::decodeSignature(packet)) {
				fprintf(stderr, "signature check failed\n");
			}
			else _temp.push_back(Serialize::from<Template>(packet));

			// add map section
			auto* sec = _list->push([=]() {
				// show if filename contains substring
				bool _ = name.find(_search->input.get()) != std::string::npos;
				if (_) _count++;
				return _;
			});
			
			// add map info
			sec->line("edit.map.file.k");
			sec->extra("edit.map.file.v", 0.4ps, sf::Color::Magenta);
			sec->line("edit.map.name.k");
			sec->extra("edit.map.name.v", 0.4ps);
			sec->line("edit.map.author.k");
			sec->extra("edit.map.author.v", 0.4ps, sf::Color::Yellow);

			// set arguments
			sec->args = {
				{ "file", name },
				{ "name", _temp.back().header.name },
				{ "auth", _temp.back().header.auth }
			};

			// add selection callback
			sec->onEvent([=](const ui::Event& evt) {
				if (auto data = evt.get<ui::Event::MousePress>()) {
					if (data->button == sf::Mouse::Button::Left) {
						// select the map
						select(sec, &_temp.back());

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
};