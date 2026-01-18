#pragma once
#include "shared.hpp"
#include "ui/text.hpp"
#include "assets.hpp"
#include "game/troop.hpp"

namespace Values {
	/// Move undo button icon.
	extern const sf::IntRect undo_icon;
	/// Move redo button icon.
	extern const sf::IntRect redo_icon;
	/// Turn finish button icon.
	extern const sf::IntRect next_icon;

	/// Troop buying action texture.
	extern const sf::IntRect buy_build;
	/// Building buying action texture.
	extern const sf::IntRect buy_troop;

	/// Income arrow textures.
	extern const sf::IntRect income_arrow[3];
	/// Income colors.
	extern const sf::Color income_color[3];

	/// Cooldown annotation color.
	extern const sf::Color cooldown_color;

	/// Coin currency icon.
	extern const sf::IntRect coin_icon;
	/// Peach currency icon.
	extern const sf::IntRect peach_icon;
	/// Berry currency icon.
	extern const sf::IntRect berry_icon;

	/// Interface digit textures.
	extern const sf::IntRect digits[11];
	/// Stunned skill digit replacement.
	extern const sf::IntRect stun_digit;
	/// Dead region skill digit replacement.
	extern const sf::IntRect dead_digit;
	/// Insufficient resources digit replacement.
	extern const sf::IntRect insufficient_digit;
	/// Timed-out skill cooldown digit.
	extern const sf::IntRect timeout_digit;

	/// Game panel text settings.
	extern const ui::TextSettings panel_text;
	/// Resource bar text settings.
	extern const ui::TextSettings resource_text;
	/// Game state viewer text settings.
	extern const ui::TextSettings state_text;
	/// Game chat text settings.
	extern const ui::TextSettings chat_text;
	/// Splash text settings.
	extern const ui::TextSettings splash_text;

	/// Unknown player nametag color.
	extern const sf::Color unknown_color;
	/// Host nametag color.
	extern const sf::Color host_color;
};