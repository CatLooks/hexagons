#include "game/moves/entity_effect.hpp"
#include "game/map.hpp"

namespace Moves {
	/// Constructs entity effect move.
	EntityEffect::EntityEffect(sf::Vector2i pos, EffectType effect, int peach)
		: pos(pos), effect(effect), peach(peach), a_before(false) { };

	/// Applies the move.
	void EntityEffect::onApply(Map* map) {
		// get entity at position
		Hex* hex = map->at(pos);
		if (!hex) return;
		Entity* e = hex->entity();
		if (!e) return;

		// store effect state
		a_before = e->hasEffect(effect);

		// apply effect to entity
		e->addEffect(effect);

		// subtract effect cost
		if (hex->region())
			hex->region()->peach -= peach;
	};

	/// Reverts the move.
	void EntityEffect::onRevert(Map* map) {
		// ignore if effect was already applied before
		if (a_before) return;

		// get entity at position
		Hex* hex = map->at(pos);
		if (!hex) return;
		Entity* e = hex->entity();
		if (!e) return;

		// remove effect from entity
		e->removeEffect(effect);

		// add effect cost
		if (hex->region())
			hex->region()->peach += peach;
	};
};