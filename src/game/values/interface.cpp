#include "game/values/interface.hpp"

namespace Values {
	/// Move undo button icon.
	const sf::IntRect undo_icon = coords(7, 2);
	/// Move redo button icon.
	const sf::IntRect redo_icon = coords(8, 2);
	/// Turn finish button icon.
	const sf::IntRect next_icon = coords(9, 2);

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
	/// Income colors.
	const sf::Color income_color[3] = {
		sf::Color(0  , 255, 0), // up
		sf::Color(255, 255, 0), // zero
		sf::Color(255, 0  , 0), // down
	};

	/// Cooldown annotation color.
	const sf::Color cooldown_color = sf::Color(255, 32, 64);

	/// Coin currency icon.
	const sf::IntRect coin_icon = coords(7, 1);
	/// Peach currency icon.
	const sf::IntRect peach_icon = coords(8, 1);
	/// Berry currency icon.
	const sf::IntRect berry_icon = coords(9, 1);

	/// Interface digit textures.
	const sf::IntRect digits[11] = {
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
		coords(9, 3, 2, 2),
	};
	/// Stunned skill digit replacement.
	const sf::IntRect stun_digit = coords(9, 3, 3, 2);
	/// Dead region skill digit replacement.
	const sf::IntRect dead_digit = coords(9, 3, 0, 3);
	/// Insufficient resources digit replacement.
	const sf::IntRect insufficient_digit = coords(9, 3, 1, 3);
	/// Timed-out skill cooldown digit.
	const sf::IntRect timeout_digit = coords(9, 3, 2, 3);

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
	/// Game state viewer text settings.
	const ui::TextSettings state_text = {
		assets::font, 30, sf::Color::White,
		sf::Color::Black, 3
	};
	/// Game chat text settings.
	const ui::TextSettings chat_text = {
		assets::font, 20, sf::Color::White,
		sf::Color::Black, 2
	};

	/// Unknown player nametag color.
	const sf::Color unknown_color = sf::Color(32, 32, 32);
	/// Host nametag color.
	const sf::Color host_color = sf::Color(192, 0, 0);
};