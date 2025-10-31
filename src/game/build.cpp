#include "game/build.hpp"

namespace Name {
	/// Returns a request value for building name.
	const char* build(Build::Type id) {
		static const char* table[Build::Count] {
			"@!build.castle",
			"@!build.farm",
			"@!build.mine",
			"@!build.tower",
			"@!build.beacon",
			"@!build.fort"
		};
		return (id >= 0 && id < Build::Count)
			? table[id] : "@!build.bad";
	};
	/// Returns a request value for building name.
	const char* build(const Build& entity) {
		return build(entity.type);
	};
};