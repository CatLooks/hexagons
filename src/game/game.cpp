#include "game/game.hpp"
#include "game/values/shared.hpp"
#include "game/logic/skill_list.hpp"
#include "mathext.hpp"

/// Default tile position for deselection click.
const sf::Vector2i Game::unselected = { -1, -1 };

/// Queues a call for the next frame.
void Game::queueCall(Delegate<void()>::Action call) {
	_queue.add(call);
};

/// Constructs a game object.
Game::Game(ui::Layer* layer, gameui::Panel* panel)
	: _layer(layer), _camera(layer, &map.camera, 17.f / 16), _panel(panel)
{
	// setup camera
	_camera.minZoom = 0.5f;
	_camera.maxZoom = 2.0f;
	layer->infinite = true;

	// add queued call handler
	layer->onRecalculate([=](const sf::Time& _) {
		_queue.invoke();
		_queue.clear();
	});

	// add tile selection handler
	layer->onEvent([=](const ui::Event& evt) {
		if (auto data = evt.get<ui::Event::MousePress>()) {
			if (data->button == sf::Mouse::Button::Left) {
				// process tile (de)selection
				sf::Vector2i pos = mouseToHex(data->position);
				click(pos);
				updateMenu();
				return true;
			};
		};
		if (auto data = evt.get<ui::Event::KeyPress>()) {
			if (data->key == sf::Keyboard::Key::Escape) {
				// deselect anything selected
				click(unselected);
				updateMenu();
				return true;
			};
		};
		return false;
	});

	// add camera zoom handler
	layer->onEvent([=](const ui::Event& evt) {
		if (auto data = evt.get<ui::Event::MouseWheel>()) {
			_camera.scroll(-data->delta, ui::window.mouse(), ui::window.size());
			return true;
		};
		return false;
	});

	// add camera pan handler
	layer->onUpdate([=](const sf::Time& delta) {
		// set layer scale
		layer->setArea(ui::DimVector(1es, 1es) * _camera.zoom(), { 0px, 0px, 1ps, 1ps });

		// keyboard camera pan
		if (ui::window.focused()) {
			sf::Vector2i offset;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) offset.y--;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) offset.y++;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) offset.x--;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) offset.x++;
			map.camera += sf::Vector2i(sf::Vector2f(offset * Values::k)
				* Values::panSpeed * delta.asSeconds());
		};

		// mouse camera pan
		_camera.pan(
			sf::Mouse::isButtonPressed(sf::Mouse::Button::Right),
			ui::window.mouse(), ui::window.size()
		);
	});

	// deselect when clicking on the panel
	panel->onEvent([=](const ui::Event& evt) {
		if (evt.is<ui::Event::MousePress>())
			deselectMenu();
		return true;
	});
};

/// Selects a tile at position.
void Game::selectTile(sf::Vector2i pos) {
	// deselect previous tile
	deselectTile();
	{
		// animate tile ascend
		Hex* hex = map.at(pos);
		if (hex) {
			ui::Anim* anim = ui::AnimFloat::to(&hex->elevation, 1.f, sf::seconds(0.15f));
			anim->setEasing(ui::Easings::sineIn);
			push(anim);
		};
	};

	// select new tile
	_select = pos;
	_last = pos;
};
/// Deselects a tile.
void Game::deselectTile() {
	if (_select) {
		// animate tile descend
		Hex* hex = map.at(*_select);
		if (hex) {
			ui::Anim* anim = ui::AnimFloat::to(&hex->elevation, 0.f, sf::seconds(0.1f));
			anim->setEasing(ui::Easings::sineOut);
			push(anim);
		};
	};

	// deselect tile
	_select = {};
};
/// Clicks at a tile.
void Game::click(sf::Vector2i pos) {
	Hex* hex = map.at(pos);
	if (map.isSelection()) {
		/// tile selection ///
		
		// tile not selected
		if (!hex || hex->selected != map.getSelectionIndex()) {
			deselectMenu();
			return;
		};

		// deselect if status skill
		if (skill->format == SkillDesc::Self) {
			deselectMenu();
			return;
		};

		// execute skill action
		Spread::Tile prev = { last(), map.at(last()), 0 };
		Spread::Tile next = { pos, hex, 0 };
		skill->action(map, prev, next);

		// select target tile
		deselectMenu();
		{
			map.selectRegion(next.hex->region);
			selectTile(pos);
		};
	}
	else {
		/// tile look-up ///

		// outside of map
		if (!hex) {
			// deselect the tile
			if (_select) deselectTile();
			// deselect the region
			else map.deselectRegion();
			return;
		};

		// if a region is selected
		if (const auto& region = map.selectedRegion()) {
			// check if selecting the same tile
			if (_select && *_select == pos)
				deselectTile();

			// check if in the same region
			else if (hex->region == region)
				selectTile(pos);

			// deselect tile
			else if (_select) deselectTile();

			// deselect region
			else map.deselectRegion();
			return;
		};

		// select the region
		map.selectRegion(hex->region);
	};
};

/// Cycles the building buying interface.
void Game::cycleBuild() {
	_build = (_build + 1) % (int)Values::build_shop.size();
	updateBuild();
};
/// Cycles the troop buying interface.
void Game::cycleTroop() {
	_troop = (_troop + 1) % (int)Values::troop_shop.size();
	updateTroop();
};

/// Updates the building buying interface.
void Game::updateBuild() const {
	// get target button
	if (_panel->actions().size() < 2) return;
	gameui::Action* button = _panel->actions()[0];

	// get building type
	Build::Type type = Values::build_shop[_build];

	// update texture
	button->setTexture(&assets::tilemap, Values::build_textures[type]);

	// update label
	auto* text = button->setLabel();
	text->setPath("param");
	text->param("value", Values::build_names[type]);
};

/// Updates the troop buying interface.
void Game::updateTroop() const {
	// get target button
	if (_panel->actions().size() < 2) return;
	gameui::Action* button = _panel->actions()[1];

	// get building type
	Troop::Type type = Values::troop_shop[_troop];

	// update texture
	button->setTexture(&assets::tilemap, Values::troop_textures[type]);

	// update label
	auto* text = button->setLabel();
	text->setPath("param");
	text->param("value", Values::troop_names[type]);
};

/// Deselect action buttons and cancels map selection.
void Game::deselectMenu() {
	map.stopSelection();
	for (auto* button : _panel->actions())
		button->deselect();
	skill = nullptr;
};

/// Attaches a callback to a button.
/// 
/// @param button Action button.
/// @param game Game object.
/// @param skill Action skill.
static void _attach_action(
	gameui::Action* button,
	Game* game,
	const SkillDesc* skill
) {
	button->setCall(
		[=]() {
			// deselect other selections
			game->deselectMenu();

			// trigger map selection
			size_t idx = game->map.newSelectionIndex();

			// generate tile selection
			Spread spread = skill->select({ game->last(), game->map.at(game->last()) }, idx);
			spread.apply(game->map, game->last(), skill->radius);

			// store skill description
			game->skill = skill;
		},
		[=]() {
			// aimed skill
			if (game->skill->format != SkillDesc::Self) {
				// cancel action
				game->deselectMenu();
				return;
			};

			// execute the skill
			HexRef tile = { game->last(), game->map.at(game->last()) };
			game->skill->action(game->map, tile, tile);
			game->deselectMenu();

			// deselect the tile if entity has disappeared
			if (tile.hex->free()) {
				game->queueCall([=]() {
					game->deselectTile();
					game->updateMenu();
				});
			};
		},
		gameui::Action::Select
	);
};

/// Constructs an entity menu.
/// 
/// @param game Game object.
/// @param panel Panel element.
/// @param data Menu data.
/// @param texture Entity texture.
/// @param name Entity name.
static void _construct_menu(
	Game* game,
	gameui::Panel* panel,
	const Values::SkillArray& data,
	sf::IntRect texture,
	const char* name
) {
	// construct a panel
	panel->construct(data.layout);

	// create entity preview
	{
		panel->preview()->setTexture(&assets::tilemap, texture);
		auto* text = panel->preview()->setLabel();
		text->setPath("param");
		text->param("value", name);
	};

	// construct skill buttons
	int idx = 0;
	for (auto* button : panel->actions()) {
		// set button texture
		button->setTexture(&assets::interface, Values::skill_textures[data.skills[idx]->type]);

		// set skill annotation
		button->annotate(data.skills[idx]->annotation);

		// set skill label
		auto* text = button->setLabel();
		text->setPath("param");
		text->param("value", Values::skill_names[data.skills[idx]->type]);

		// attach skill logic
		_attach_action(button, game, data.skills[idx]);
		idx++;
	};
	panel->recalculate();
};

/// Constructs a region UI panel.
void Game::regionMenu(const Region& region, bool targeted) {
	_panel->construct(Values::SkillArray::L22);

	// create region tile preview
	{
		gameui::preview::hex.team = region.team;
		_panel->preview()->setDraw(gameui::preview::draw);
		auto* text = _panel->preview()->setLabel();
		text->setPath("gp.team");
		text->param("team", Values::hex_names[region.team]);
		text->setColor(Values::hex_colors[region.team]);
	};

	// construct buy buttons
	{
		auto* button = _panel->actions()[2];

		// buy building button
		button->annotate(targeted ? SkillDesc::None : SkillDesc::Aim);
		button->setTexture(&assets::interface, Values::buy_build);
		auto* text = button->setLabel();
		text->setPath("gp.buy_build");

		// attach buy build skill
		_attach_action(button, this, targeted ? &SkillList::buy_build : &SkillList::buy_build_aim);
	};
	{
		auto* button = _panel->actions()[3];

		// buy troop button
		button->annotate(targeted ? SkillDesc::None : SkillDesc::Aim);
		button->setTexture(&assets::interface, Values::buy_troop);
		auto* text = button->setLabel();
		text->setPath("gp.buy_troop");

		// attach buy build skill
		_attach_action(button, this, targeted ? &SkillList::buy_troop : &SkillList::buy_troop_aim);
	};

	// annotate selection buttons
	_panel->actions()[0]->annotate(SkillDesc::Swap);
	_panel->actions()[1]->annotate(SkillDesc::Swap);

	// add callbacks to selection buttons
	_panel->actions()[0]->setCall([=]() { cycleBuild(); }, nullptr, gameui::Action::Click);
	_panel->actions()[1]->setCall([=]() { cycleTroop(); }, nullptr, gameui::Action::Click);

	// update entity previews
	updateBuild();
	updateTroop();

	// recalculate panel
	_panel->recalculate();
};

/// Constructs a troop UI panel.
void Game::troopMenu(const Troop& troop) {
	_construct_menu(
		this, _panel,
		Values::troop_skills[troop.type],
		Values::troop_textures[troop.type],
		Values::troop_names[troop.type]
	);
};
/// Constructs a building UI panel.
void Game::buildMenu(const Build& build) {
	_construct_menu(
		this, _panel,
		Values::build_skills[build.type],
		Values::build_textures[build.type],
		Values::build_names[build.type]
	);
};
/// Constructs a plant UI panel.
void Game::plantMenu(const Plant& plant) {
	_construct_menu(
		this, _panel,
		Values::plant_skill,
		Values::plant_textures[plant.type],
		Values::plant_names[plant.type]
	);
};

/// Constructs a troop UI panel.
void Game::hexMenu(const Hex& hex) {
	// troop ui
	if (hex.troop) troopMenu(*hex.troop);
	// build ui
	else if (hex.build) buildMenu(*hex.build);
	// plant ui
	else if (hex.plant) plantMenu(*hex.plant);
	// targeted region ui
	else regionMenu(*hex.region, true);
};

/// Closes any open menus.
void Game::closeMenu() {
	_panel->construct(Values::SkillArray::None);
	_panel->recalculate();
};

/// Updates menu state after a click.
void Game::updateMenu() {
	// open hex menu
	if (_select)
		hexMenu(*map.at(*_select));

	// open untargeted region menu
	else if (const auto& region = map.selectedRegion())
		regionMenu(*region);

	// close menus
	else closeMenu();
};

/// Returns last click position.
sf::Vector2i Game::last() const {
	return _last;
};

/// Returns hex coordinates at a mouse position.
sf::Vector2i Game::mouseToHex(sf::Vector2i mouse) const {
	// convert to world coordinates
	mouse += map.camera;

	// calculate y
	int y, ymod;
	ext::idivmod(mouse.y, Values::tileOff.y).into(y, ymod);

	// shift x if odd row
	mouse -= Values::rowOffset(y);

	// calculate x
	int x, xmod;
	ext::idivmod(mouse.x, Values::tileSize.x).into(x, xmod);

	// get base tile position
	sf::Vector2i pos = { x, y };

	// check for neighbor hops
	if (y < Values::tileSize.y - Values::tileOff.y) {
		// check neighboring hex
		if (xmod < Values::tileSize.x / 2) {
			// upper-left neighbor
			if (ymod < Values::tileUnit / 4 - xmod / 2)
				pos = map.neighbor(pos, Map::UpperLeft);
		}
		else {
			// upper-right neighbor
			if (ymod < xmod / 2 - Values::tileUnit / 4)
				pos = map.neighbor(pos, Map::UpperRight);
		};
	};
	return pos;
};

/// Draws the map.
void Game::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
	map.draw(target);
};