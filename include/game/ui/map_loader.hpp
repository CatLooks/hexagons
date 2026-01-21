#pragma once

// include dependencies
#include "dev/dev_panel.hpp"
#include "game/template.hpp"
#include "resource_table.hpp"
#include "assets.hpp"
#include "ui.hpp"

namespace gameui {
	/// Map loader element.
	class Loader : public ui::Panel {
	private:
		/// Game controller reference.
		Game* _game;
		/// Region resource table reference.
		Table* _table;

		/// Map list.
		dev::Panel* _list;
		/// Selected map section.
		dev::Section* _select_sec;
		/// Selected map template.
		const Template* _select_temp;
		/// Selected map filename.
		std::string _select_file;

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

		Field* _f_file; /// File field.
		Field* _f_name; /// Name field.
		Field* _f_auth; /// Author field.

		Field* _f_size;  /// Size field.
		Field* _f_re;    /// Resize field.

		Field* _f_troop; /// Troop field.
		Field* _f_build; /// Build field.
		Field* _f_plant; /// Plant field.
		Field* _f_reg;   /// Region field.

		/// Map templates.
		std::list<Template> _temp;

	public:
		/// Construct a map loader.
		///
		/// @param game Game controller reference.
		/// @param table Region resources table.
		Loader(Game* game, Table* table);

		/// Constructs a map from template.
		void construct();

		/// Selects a map from the list.
		/// 
		/// @param section Map section.
		/// @param temp Map template.
		void select(dev::Section* section, const Template* temp);

		/// Reloads map list.
		void reload();

		/// Whether any input field is active.
		bool input() const;
	};

	/// Map loader grabber button.
	class Grabber : public ui::Button {
	private:
		/// Whether running during the first frame.
		bool _first = true;

	public:
		/// Constructs a grabber button.
		///
		/// @param loader Map loader reference.
		Grabber(Loader* loader);
	};
};