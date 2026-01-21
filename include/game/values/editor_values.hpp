#pragma once

// include dependencies
#include "shared.hpp"

namespace Values {
	/// Returns a tile in 16x16 texture map.
	sf::IntRect minicoords(int x, int y);

	/// Editor void tile button icon.
	extern const sf::IntRect edit_tile_void;
	/// Editor ground tile button icon.
	extern const sf::IntRect edit_tile_ground;
	/// Editor water tile button icon.
	extern const sf::IntRect edit_tile_water;
	/// Editor tile team button icon.
	extern const sf::IntRect edit_team;

	/// Editor troop button icon.
	extern const sf::IntRect edit_troop;
	/// Editor build button icon.
	extern const sf::IntRect edit_build;
	/// Editor plant button icon.
	extern const sf::IntRect edit_plant;
	/// Editor none button icon.
	extern const sf::IntRect edit_none;

	/// Editor hitpoit increase button icon.
	extern const sf::IntRect edit_hp_up;
	/// Editor hitpoit decrease button icon.
	extern const sf::IntRect edit_hp_down;
	/// Editor hitpoit max button icon.
	extern const sf::IntRect edit_hp_max;

	/// Editor troop button icon.
	extern const sf::IntRect edit_copy;
	/// Editor build button icon.
	extern const sf::IntRect edit_cut;
	/// Editor plant button icon.
	extern const sf::IntRect edit_paste;
};