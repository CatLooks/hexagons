#pragma once

// include dependencies
#include "dev/dev_panel.hpp"
#include "game/logic/turn_logic.hpp"
#include "game/sync/messages.hpp"

namespace gameui {
	/// Capture progress view element.
	class Progress : public dev::Section {
	protected:
		/// Argument lists for each row.
		std::vector<ui::Text::List> args;
		/// Displayed teams.
		std::vector<Region::Team> teams;

	public:
		/// Constructs a progress view element.
		/// 
		/// @param count Player count.
		Progress();

		/// Constructs player info table.
		/// 
		/// @param teams Displayed teams.
		void reconstruct(const std::vector<Region::Team>& teams);

		/// Updates table data.
		/// 
		/// @param count Tile count object.
		/// @param players Player list.
		void update(const logic::TileCount& count, const std::vector<Messages::Player>& players);
	};
};