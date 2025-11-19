#include "game/values/interface.hpp"

namespace Values {
	/// Troop buying action texture.
	const sf::IntRect buy_build = coords(4, 3);
	/// Building buying action texture.
	const sf::IntRect buy_troop = coords(5, 3);

	/// Income arrow textures.
	const sf::IntRect income_arrow[3] = {
		coords(7, 0), // up
		coords(8, 0), // zero
		coords(9, 0), // down
	};

	/// Troop skills textures.
	const sf::IntRect troop_skills[static_cast<int>(SkillType::Count)] = {
		coords(0, 1),
		coords(1, 1),
		coords(2, 1),
		coords(3, 1),
		coords(4, 1),
		coords(5, 1),
		coords(0, 2),
		coords(1, 2),
		coords(2, 2),
		coords(3, 2),
		coords(4, 2),
		coords(5, 2),
		coords(0, 3),
		coords(1, 3)
	};

	/// Troop effect textures.
	const sf::IntRect troop_effects[static_cast<int>(EffectType::Count)] = {
		coords(8, 3, 0, 0),
		coords(8, 3, 1, 0),
		coords(8, 3, 2, 0),
		coords(8, 3, 3, 0),
	};

	/// Action annotation icon textures.
	const sf::IntRect annotations[static_cast<int>(Annotation::Count)] = {
		coords(8, 3, 0, 1),
		coords(8, 3, 1, 1),
		coords(8, 3, 2, 1),
		coords(8, 3, 3, 1),
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
};