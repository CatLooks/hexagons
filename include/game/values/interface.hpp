#pragma once
#include "shared.hpp"
#include "ui/text.hpp"
#include "assets.hpp"
#include "game/troop.hpp"

namespace Values {
	/// Troop buying action texture.
	extern const sf::IntRect buy_build;
	/// Building buying action texture.
	extern const sf::IntRect buy_troop;

	/// Income arrow textures.
	extern const sf::IntRect income_arrow[3];

	/// Coin currency icon.
	extern const sf::IntRect coin_icon;
	/// Peach currency icon.
	extern const sf::IntRect peach_icon;
	/// Berry currency icon.
	extern const sf::IntRect berry_icon;

	/// Interface digit textures.
	extern const sf::IntRect digits[10];

	/// Game panel text settings.
	extern const ui::TextSettings panel_text;
	/// Resource bar text settings.
	extern const ui::TextSettings resource_text;
};