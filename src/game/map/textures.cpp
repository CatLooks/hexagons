#include "game/map/textures.hpp"

namespace Textures {
	/// Returns a 64x64 texture map.
	static inline sf::IntRect coords(int x, int y) {
		return { { x * 64, y * 64 }, { 64, 64 } };
	};

	/// Missing texture replacement.
	const sf::IntRect missing = coords(9, 3);

	/// Returns the texture of a hex team.
	sf::IntRect hex(Hex::Team id) {
		return (id >= 0 && id < Hex::Count)
			? coords(id, 0) : missing;
	};

	/// Returns the texture of a troop type.
	sf::IntRect troop(Troop::Type id) {
		return (id >= 0 && id < Troop::Count)
			? coords(id, 1) : missing;
	};

	/// Returns the texture of a building type.
	sf::IntRect build(Build::Type id) {
		return (id >= 0 && id < Build::Count)
			? coords(id, 2) : missing;
	};

	/// Returns the texture of a plant type.
	sf::IntRect plant(Plant::Type id) {
		return (id >= 0 && id < Plant::Count)
			? coords(id, 3) : missing;
	};
};