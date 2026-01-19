#include "game/ui/editor.hpp"
#include "game/game.hpp"

#include "game/values/hex_values.hpp"
#include "game/values/troop_values.hpp"
#include "game/values/build_values.hpp"
#include "game/values/plant_values.hpp"

/// Editor button side length.
const ui::Dim Editor::side = 64px;

/// Move selector button texture map.
const ui::Panel::Map texture = ui::Panel::Map::rect
	(&assets::interface, Values::coords(0, 0), { 6, 6 }, 1);

/// Generate a new editor button.
/// 
/// @param icon Button icon.
static ui::Button* new_button(sf::IntRect icon) {
	auto* button = new ui::Button(texture, { Editor::side, Editor::side });
	auto* image = button->setIcon(&assets::interface, icon);
	image->size() *= 2;
	return button;
};

/// Construct editor environment.
Editor::Editor(Game* game, ui::Layer* text_layer): _game(game) {
	// adds a shortcut for the button
	auto add_shortcut = [](ui::Button* button, sf::Keyboard::Key key, bool control) {
		button->onEvent([=](const ui::Event& evt) {
			if (auto data = evt.get<ui::Event::KeyPress>()) {
				// check if key matches
				if (data->key == key && data->control == control) {
					button->force();
					return true;
				};
			};

			// ignore event
			return false;
		});
	};

	// layout list
	std::vector<ui::Button*> list;

	// layouting function
	auto layout = [&](ui::Element* parent, bool x) {
		// get element parallel offset
		auto off_par = [&](ui::Element* elem) -> ui::Dim&
			{ return x ? elem->position().y : elem->position().x; };
		// get element perpendicular offset
		auto off_per = [&](ui::Element* elem) -> ui::Dim&
			{ return x ? elem->position().x : elem->position().y; };

		// get total width / height
		const ui::Dim pad = 8px;

		// initial offset
		ui::Dim off = (side + pad) * (1.f - list.size()) / 2.f;

		// layout buttons
		for (const auto& button : list) {
			if (button) {
				off_par(button) = 0.5as + off;
				off_per(button) = 1as;

				// add to parent
				parent->add(button);

				// mask all clicks through
				button->onEvent([=](const ui::Event& evt) {
					if (auto data = evt.get<ui::Event::MousePress>())
						return data->button == sf::Mouse::Button::Left;
					return false;
				});
			};
			off += side + pad;
		};
	};

	// layout directions
	constexpr bool HORIZONTAL = 0;
	constexpr bool VERTICAL = 1;

	// tile panel
	ui::Element* tile_panel = new ui::Element;
	tile_panel->bounds = { 1as, 0.5as, side, 1ps };
	{
		// void tile
		auto* tv = new_button(Values::edit_tile_void);
		add_shortcut(tv, sf::Keyboard::Key::Z, false);
		tv->validate([this]() {
			auto tile = tileref();
			if (!tile.hex) return false;
	
			// change type & remove any entities
			tile.hex->type = Hex::Void;
			tile.hex->leave();
			_game->map.removeEntity(tile.hex);

			// update resource table
			_res_table->sync(nullptr);
			return true;
		});

		// water tile
		auto* tw = new_button(Values::edit_tile_water);
		add_shortcut(tw, sf::Keyboard::Key::X, false);
		tw->validate([this]() {
			auto tile = tileref();
			if (!tile.hex) return false;

			// change type & remove any entities
			tile.hex->type = Hex::Water;
			tile.hex->leave();
			_game->map.removeEntity(tile.hex);

			// update resource table
			_res_table->sync(nullptr);
			return true;
		});

		// ground tile
		auto* tg = new_button(Values::edit_tile_ground);
		add_shortcut(tg, sf::Keyboard::Key::C, false);
		tg->validate([this]() {
			auto tile = tileref();
			if (!tile.hex) return false;

			// change type & team
			tile.hex->type = Hex::Ground;
			tile.hex->team = team;

			// create new region
			auto ref = _game->map.regions.create({ .team = team });
			tile.hex->join(ref);

			// update regions
			_game->map.updateRegions(tile, tile.hex->region(), {});

			// update resource table
			_res_table->sync(&*tile.hex->region());
			return true;
		});

		// team cycle
		auto* tt = new_button(Values::edit_team);
		add_shortcut(tt, sf::Keyboard::Key::V, false);
		tt->attach([this]() {
			team = static_cast<Region::Team>((team + 1) % Region::Count);
			updateLabels();
		});

		// generate layout
		list = { tv, tw, tg, tt };
		layout(tile_panel, VERTICAL);

		// add team color label
		_team_text = tt->setLabel(Values::panel_text);
		_team_text->pos = ui::Text::ShrinkY;
		_team_text->autosize = true;
		_team_text->position() = { -1ts - 4px, 0.5as };
		_team_text->setPath("param");
	};
	add(tile_panel);

	// entity panel
	ui::Element* entity_panel = new ui::Element;
	entity_panel->bounds = { 0.5as, 1as, 1ps, side };
	{
		// cycle troops
		auto* et = new_button(Values::edit_troop);
		add_shortcut(et, sf::Keyboard::Key::Num1, false);
		et->validate([this]() {
			// get tile
			auto tile = tileref();
			if (!tile.hex) return false;

			// ignore if tile is not solid
			if (!tile.hex->solid()) return false;

			// cycle troop
			if (tile.hex->troop) {
				auto& type = tile.hex->troop->type;
				type = static_cast<Troop::Type>((type + 1) % Troop::Count);
				tile.hex->troop->hp = tile.hex->troop->max_hp();
			}
			else {
				// create first troop
				Troop troop;
				troop.pos = tile.pos;
				troop.type = Troop::Farmer;
				troop.hp = troop.max_hp();
				_game->map.setTroop(troop);
			};
			return true;
		});

		// cycle builds
		auto* eb = new_button(Values::edit_build);
		add_shortcut(eb, sf::Keyboard::Key::Num2, false);
		eb->validate([this]() {
			// get tile
			auto tile = tileref();
			if (!tile.hex) return false;

			// ignore if tile is not solid
			if (!tile.hex->solid()) return false;

			// cycle build
			if (tile.hex->build) {
				auto& type = tile.hex->build->type;
				type = static_cast<Build::Type>((type + 1) % Build::Count);
				tile.hex->build->hp = tile.hex->build->max_hp();
			}
			else {
				// create first troop
				Build build;
				build.pos = tile.pos;
				build.type = Build::Farm;
				build.hp = build.max_hp();
				_game->map.setBuild(build);
			};
			return true;
		});

		// cycle plants
		auto* ep = new_button(Values::edit_plant);
		add_shortcut(ep, sf::Keyboard::Key::Num3, false);
		ep->validate([this]() {
			// get tile
			auto tile = tileref();
			if (!tile.hex) return false;

			// ignore if tile is not solid
			if (!tile.hex->solid()) return false;

			// cycle plant
			if (tile.hex->plant) {
				auto& type = tile.hex->plant->type;
				type = static_cast<Plant::Type>((type + 1) % Plant::Count);
			}
			else {
				// create first troop
				Plant plant;
				plant.pos = tile.pos;
				plant.type = Plant::Bush;
				_game->map.setPlant(plant);
			};
			return true;
		});

		// remove entities
		auto* en = new_button(Values::edit_none);
		add_shortcut(en, sf::Keyboard::Key::Num4, false);
		en->validate([this]() {
			// get tile
			auto tile = tileref();
			if (!tile.hex) return false;

			// remove entity
			if (tile.hex->entity()) {
				_game->map.removeEntity(tile.hex);
				return true;
			};
			return false;
		});

		// health up
		auto* hu = new_button(Values::edit_hp_up);
		add_shortcut(hu, sf::Keyboard::Key::PageUp, false);
		hu->validate([this]() {
			// get tile
			auto tile = tileref();
			if (!tile.hex) return false;

			// get entity
			if (Entity* ent = tile.hex->entity()) {
				ent->hp++;
				return true;
			};
			return false;
		});

		// health down
		auto* hd = new_button(Values::edit_hp_down);
		add_shortcut(hd, sf::Keyboard::Key::PageDown, false);
		hd->validate([this]() {
			// get tile
			auto tile = tileref();
			if (!tile.hex) return false;

			// get entity
			if (Entity* ent = tile.hex->entity()) {
				if (ent->hp > 1) {
					ent->hp--;
					return true;
				};
			};
			return false;
		});

		// health reset
		auto* hm = new_button(Values::edit_hp_max);
		add_shortcut(hm, sf::Keyboard::Key::Home, false);
		hm->validate([this]() {
			// get tile
			auto tile = tileref();
			if (!tile.hex) return false;

			// get entity
			if (Entity* ent = tile.hex->entity()) {
				ent->hp = ent->max_hp();
				return true;
			};
			return false;
		});

		// entity copy function
		auto copy = [this]() -> bool {
			// get tile
			auto tile = tileref();
			if (!tile.hex) return false;

			// copy tile entity
			if (Entity* ent = tile.hex->entity()) {
				// copy entity info
				if (tile.hex->troop) entity = *tile.hex->troop;
				if (tile.hex->build) entity = *tile.hex->build;
				if (tile.hex->plant) entity = *tile.hex->plant;

				// update labels
				updateLabels();
				return true;
			};
			return false;
		};

		// copy entity
		auto* cc = new_button(Values::edit_copy);
		add_shortcut(cc, sf::Keyboard::Key::C, true);
		cc->validate(copy);

		// paste entity
		auto* cv = new_button(Values::edit_paste);
		add_shortcut(cv, sf::Keyboard::Key::V, true);
		cv->validate([this]() {
			// ignore if nothing in clipboard
			if (std::holds_alternative<Moves::Empty>(entity))
				return false;

			// get tile
			auto tile = tileref();
			if (!tile.hex) return false;

			// ignore if tile is not solid
			if (!tile.hex->solid()) return false;

			// set new entity
			if (auto* data = std::get_if<Troop>(&entity)) {
				data->pos = tile.pos;
				_game->map.setTroop(*data);
			};
			if (auto* data = std::get_if<Build>(&entity)) {
				data->pos = tile.pos;
				_game->map.setBuild(*data);
			};
			if (auto* data = std::get_if<Plant>(&entity)) {
				data->pos = tile.pos;
				_game->map.setPlant(*data);
			};
			return true;
		});

		// cut entity
		auto* cx = new_button(Values::edit_cut);
		add_shortcut(cx, sf::Keyboard::Key::X, true);
		cx->validate([this, copy]() {
			// copy entity
			if (!copy()) return false;

			// get tile
			auto tile = tileref();
			if (!tile.hex) return false;

			// remove entity
			_game->map.removeEntity(tile.hex);
			return true;
		});

		// generate layout
		list = {
			hu, hd, hm,
			nullptr,
			et, eb, ep, en,
			nullptr,
			cc, cv, cx
		};
		layout(entity_panel, HORIZONTAL);

		// add current entity label
		_show_text = hd->setLabel(Values::panel_text);
		_show_text->pos = ui::Text::ShrinkY;
		_show_text->autosize = true;
		_show_text->position() = { 0.5as, -1ts - 4px };
		_show_text->setPath("edit.entity");

		// add entity clipboard label
		_clip_text = cv->setLabel(Values::panel_text);
		_clip_text->pos = ui::Text::ShrinkY;
		_clip_text->autosize = true;
		_clip_text->position() = { 0.5as, -1ts - 4px };
		_clip_text->setPath("edit.entity");
	};
	add(entity_panel);

	// add resource panel
	_res_table = new gameui::Table;
	_res_table->position() = { 0as, 1as };
	_res_table->size().x = 300px;
	text_layer->add(_res_table);

	// add current entity label update
	onUpdate([=](const sf::Time&) {
		// get tile
		auto tile = tileref();
		if (tile.hex && tile.hex->entity()) {
			// display entites
			if (tile.hex->troop) updateEntity(_show_text, *tile.hex->troop);
			if (tile.hex->build) updateEntity(_show_text, *tile.hex->build);
			if (tile.hex->plant) updateEntity(_show_text, *tile.hex->plant);
		}
		// display nothing
		else updateEntity(_show_text, {});
	});

	// initial label update
	updateLabels();
};

/// Updates entity label.
void Editor::updateEntity(ui::Text* text, const Moves::EntState& entity) {
	std::string name = "?";
	int hp = 0, max = -1;

	// get entity data
	bool empty = true;
	if (auto* data = std::get_if<Troop>(&entity)) {
		name = Values::troop_names[data->type];
		hp = data->hp;
		max = data->max_hp();
	};
	if (auto* data = std::get_if<Build>(&entity)) {
		name = Values::build_names[data->type];
		hp = data->hp;
		max = data->max_hp();
	};
	if (auto* data = std::get_if<Plant>(&entity)) {
		name = Values::plant_names[data->type];
		hp = data->hp;
		max = data->max_hp();
	};

	// set text parameters
	if (max == -1)
		text->deactivate();
	else {
		text->activate();
		text->param("name", name);
		text->param("hp", ext::str_int(hp));
		text->param("max", ext::str_int(max));
		text->recalculate();
	};
};

/// Update text labels.
void Editor::updateLabels() {
	// tile team
	_team_text->param("value", Values::hex_names[team]);
	_team_text->setColor(Values::hex_colors[team]);
	_team_text->recalculate();

	// entity clipboard
	updateEntity(_clip_text, entity);
};

/// Selects a region.
void Editor::region(Region* reg) {
	_res_table->sync(reg);
};

/// Returns selected tile.
HexRef Editor::tileref() const {
	return _game->_select
		? _game->map.atref(*_game->_select)
		: HexRef{ nullptr, Game::unselected };
};

/// Whether any input field is active.
bool Editor::input() const {
	return _res_table->input();
};