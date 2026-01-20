#pragma once

// include dependencies
#include "dev/dev_panel.hpp"
#include "game/template.hpp"
#include "assets.hpp"
#include "ui.hpp"

namespace gameui {
	/// Map loader element.
	class Loader : public ui::Panel {
	private:
		/// Game controller reference.
		Game* _game;

		/// Map list.
		dev::Panel* _list;
		/// Selected map section.
		dev::Section* _select_sec;
		/// Selected map template.
		const Template* _select_temp;

		/// Filtered map count.
		size_t _count = 0;
		/// Map search.
		ui::TextField* _search;
		/// Filter count label.
		ui::Text* _filter;
		/// Load button.
		ui::Button* _load_btn;
		/// Load button label.
		ui::Text* _load_txt;

		/// Map templates.
		std::list<Template> _temp;

	public:
		/// Construct a map loader.
		///
		/// @param game Game controller reference.
		Loader(Game* game);

		/// Constructs a map from template.
		void construct();

		/// Selects a map from the list.
		/// 
		/// @param section Map section.
		/// @param temp Map template.
		void select(dev::Section* section, const Template* temp);

		/// Reloads map list.
		void reload();
	};
};