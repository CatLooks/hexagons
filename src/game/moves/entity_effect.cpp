#include "game/moves/entity_effect.hpp"
#include "game/map.hpp"

namespace Moves {
	/// Constructs entity effect move.
	EntityEffect::EntityEffect(sf::Vector2i pos, EffectType effect, bool before)
		: pos(pos), effect(effect), before(before) {};

	/// Applies the move.
	void EntityEffect::onApply(Map* map) {
		// get entity at position
		Hex* hex = map->at(pos);
		if (!hex) return;
		Entity* e = hex->entity();
		if (!e) return;

		// apply effect to entity
		e->addEffect(effect);
	};

	/// Reverts the move.
	void EntityEffect::onRevert(Map* map) {
		// ignore if effect was already applied before
		if (before) return;

		// get entity at position
		Hex* hex = map->at(pos);
		if (!hex) return;
		Entity* e = hex->entity();
		if (!e) return;

		// remove effect from entity
		e->removeEffect(effect);
	};
};