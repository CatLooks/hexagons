#include "game/draw/troop_draw.hpp"

namespace Draw {
	/// Draws a troop.
	void troopEntity(const Tile& tile, ui::RenderBuffer& buffer) {
		// ignore if no troop
		if (!tile.hex->troop) return;
		const Troop& troop = *tile.hex->troop;

		// draw troop
		buffer.quad({ tile.origin, Values::tileSize }, Values::troop_textures[troop.type]);
		{
			HealthBar {
				troop.hp,
				logic::troop_hp[troop.type],
				troop.hasEffect(EffectType::Poisoned)
			}.draw(tile.origin, buffer);
		};
		buffer.forward(&assets::tilemap);
	};

	/// Constructs a health bar object.
	HealthBar::HealthBar(int hp, int max, bool poison)
		: hp(hp), max(max), poison(poison) {};

	/// Draws health bar above an entity.
	void HealthBar::draw(sf::Vector2i pos, ui::RenderBuffer& buffer) const {
		// get health bar texture
		sf::IntRect texture = Values::hp_bar(hp, max, poison);

		// draw health bar panel
		buffer.quad(
			{
				{
					pos.x + (Values::tileSize.x - texture.size.x * Values::k) / 2,
					pos.y + Values::tileSize.x / 32
				},
				texture.size * Values::k
			},
			texture
		);
	};
};