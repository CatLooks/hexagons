#include "game/values/editor_values.hpp"

namespace Values {
	/// Returns a tile in 32x32 texture map.
	sf::IntRect minicoords(int x, int y) {
		return { { x * 32, y * 32 }, { 32, 32 } };
	};

	/// Editor void tile button icon.
	const sf::IntRect edit_tile_void = minicoords(8, 0);
	/// Editor ground tile button icon.
	const sf::IntRect edit_tile_ground = minicoords(9, 0);
	/// Editor water tile button icon.
	const sf::IntRect edit_tile_water = minicoords(10, 0);
	/// Editor tile team button icon.
	const sf::IntRect edit_team = minicoords(15, 6);
	
	/// Editor troop button icon.
	const sf::IntRect edit_troop = minicoords(8, 1);
	/// Editor build button icon.
	const sf::IntRect edit_build = minicoords(9, 1);
	/// Editor plant button icon.
	const sf::IntRect edit_plant = minicoords(10, 1);
	/// Editor none button icon.
	const sf::IntRect edit_none = minicoords(14, 6);

	/// Editor hitpoit increase button icon.
	const sf::IntRect edit_hp_up = minicoords(11, 0);
	/// Editor hitpoit decrease button icon.
	const sf::IntRect edit_hp_down = minicoords(12, 0);
	/// Editor hitpoit max button icon.
	const sf::IntRect edit_hp_max = minicoords(13, 0);

	/// Editor troop button icon.
	const sf::IntRect edit_copy = minicoords(11, 1);
	/// Editor build button icon.
	const sf::IntRect edit_cut = minicoords(12, 1);
	/// Editor plant button icon.
	const sf::IntRect edit_paste = minicoords(13, 1);
};