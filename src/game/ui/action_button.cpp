#include "game/ui/action_button.hpp"

namespace gameui {
	/// Action button size.
	const ui::Dim Action::size = (float)(64 * Values::k);

	/// Action button texture maps.
	const ui::Panel::Map Action::textures[2] = {
		ui::Panel::Map::rect(&assets::interface, Values::coords(0, 0), { 6, 6 }, 2),
		ui::Panel::Map::rect(&assets::interface, Values::coords(1, 0), { 6, 6 }, 2),
	};

	/// Constructs an empty action button.
	Action::Action(int hint) : ui::Panel(textures[0]), _hint(hint) {
		// set action button size
		bounds.size = { size, size };

		// reset padding
		padding.set(0);

		// add hint
		if (hint >= 0 && hint <= 9) {
			ui::Image* himg = new ui::Image(&assets::interface, Values::digits[hint]);
			himg->bounds = { { 1es / 8, 1es / 8 }, Values::iconSize };
			add(himg);
		};
	};

	/// Clears the action button.
	void Action::clear() {
		ui::Panel::clear();
		ui::Panel::clears();
		_draw = nullptr;
	};

	/// Adds an image to the action button.
	void Action::addTexture(sf::IntRect map) {
		ui::Image* dimg = new ui::Image(&assets::interface, map);
		dimg->bounds = ui::DimRect::Fill;
		adds(dimg);
	};

	/// Adds an extra draw call to the action button.
	void Action::addDraw(DrawCall call) {
		_draw = call;
	};

	/// Adds text to the action button.
	ui::Text* Action::addLabel() {
		// create empty label
		_text = new ui::Text(Values::panel_text, {});
		_text->bounds = { 0px, -24px, 1ps, 0px };
		_text->align = ui::Text::N;
		_text->pos = ui::Text::Static;

		// register text element
		add(_text);
		return _text;
	};

	/// Adds text to the action button.
	ui::Text* Action::addSubtitle() {
		// create empty label
		_sub = new ui::Text(Values::panel_text, {});
		_sub->bounds = { 0px, -44px, 1ps, 0px };
		_sub->align = ui::Text::N;
		_sub->pos = ui::Text::Static;

		// register text element
		add(_sub);
		return _sub;
	};

	/// Draws the action button.
	void Action::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
		ui::Panel::drawSelf(target, self);
		if (_draw) _draw(target, self);
	};

	namespace preview {
		// 1x1 map
		Map map = []() {
			Map map;
			map.resize({ {}, {2, 1} });
			map.at({})->type = Hex::Ground;
			return map;
		}();
		Draw::Tile tile = Draw::Tile(&map, {}, {}, Values::tileSize / 2);

		/// Draws the preview tile.
		void draw(ui::RenderBuffer& target, sf::IntRect self) {
			// set tile position
			tile.hex = map.at({});
			tile.origin = self.position + (self.size - Values::tileSize / 2) / 2;

			/* draw base    */ tile.drawBase(target);
			/* draw sides   */ tile.drawSides(target, sf::Color::Black, sf::Color::Black);
			/* draw borders */ tile.drawBorders(target, sf::Color::Black);
		};
	};
};