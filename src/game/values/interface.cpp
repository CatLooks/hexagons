#include "game/values/interface.hpp"

namespace Values {
	/// Troop buying action texture.
	const sf::IntRect buy_build = coords(6, 3);
	/// Building buying action texture.
	const sf::IntRect buy_troop = coords(6, 2);

	/// Income arrow textures.
	const sf::IntRect income_arrow[3] = {
		coords(7, 0), // up
		coords(8, 0), // zero
		coords(9, 0), // down
	};

	/// Coin currency icon.
	const sf::IntRect coin_icon = coords(7, 1);
	/// Peach currency icon.
	const sf::IntRect peach_icon = coords(8, 1);
	/// Berry currency icon.
	const sf::IntRect berry_icon = coords(9, 1);

	/// Troop effect textures.
	const sf::IntRect effects[static_cast<int>(EffectType::Count)] = {
		coords(8, 3, 0, 0), // shield
		coords(8, 3, 1, 0), // range
		coords(8, 3, 2, 0), // defense
		coords(8, 3, 3, 0), // offense
		coords(8, 3, 0, 1), // poison
		coords(8, 3, 1, 1), // stunned
	};

	/// Interface digit textures.
	const sf::IntRect digits[10] = {
		coords(9, 3, 0, 0),
		coords(9, 3, 1, 0),
		coords(9, 3, 2, 0),
		coords(9, 3, 3, 0),
		coords(9, 3, 0, 1),
		coords(9, 3, 1, 1),
		coords(9, 3, 2, 1),
		coords(9, 3, 3, 1),
		coords(9, 3, 0, 2),
		coords(9, 3, 1, 2),
	};

	/// Game panel text settings.
	const ui::TextSettings panel_text = {
		assets::font, 24, sf::Color::White,
		sf::Color::Black, 2
	};
	/// Resource bar text settings.
	const ui::TextSettings resource_text = {
		assets::font, 32, sf::Color::White,
		sf::Color::Black, 4
	};
};