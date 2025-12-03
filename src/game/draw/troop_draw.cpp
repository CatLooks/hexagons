#include "game/draw/troop_draw.hpp"

namespace Draw {
	/// Draws a troop.
	void troopEntity(const Tile& tile, ui::RenderBuffer& buffer) {
		// ignore if no troop
		if (!tile.hex->troop) return;
		const Troop& troop = *tile.hex->troop;

		// draw troop
		buffer.quad({ tile.origin, Values::tileSize }, Values::troop_textures[troop.type]);
		buffer.forward(&assets::tilemap);
	};

	/// Draws troop health bar.
	/// 
	/// @param tile Tile draw data.
	/// @param target Target render buffer.
	void troopBar(const Tile& tile, ui::RenderBuffer& buffer) {
		// ignore if no troop
		if (!tile.hex->troop) return;
		const Troop& troop = *tile.hex->troop;

		// construct troop bar
		HealthBar bar = {
			troop.hp,
			logic::troop_hp[troop.type],
			troop.hasEffect(EffectType::Poisoned)
		};
		bar.attachEffects(troop.effects);

		// draw troop bar
		bar.draw(tile.origin, buffer);
	};

	/// Constructs a health bar object.
	HealthBar::HealthBar(int hp, int max, bool poison)
		: hp(hp), max(max), poison(poison) {};

	/// Attaches a list of effects.
	HealthBar& HealthBar::attachEffects(const std::vector<EffectType>& effects) {
		this->effects = effects;
		return *this;
	};

	/// Draws health bar above an entity.
	void HealthBar::draw(sf::Vector2i pos, ui::RenderBuffer& buffer) const {
		// get health bar texture
		sf::IntRect texture = Values::hp_bar(hp, max, poison);

		// draw health bar panel
		buffer.quad({
			{
				pos.x + (Values::tileSize.x - texture.size.x * Values::k) / 2,
				pos.y + Values::tileSize.y / 32
			},
			texture.size * Values::k
		}, texture);
		buffer.forward(&assets::tilemap);

		// get x of the first icon
		int x = pos.x + Values::tileSize.x / 2
			- (int)effects.size() * Values::effects[0].size.x * Values::k / 2;

		// draw effect icons
		for (EffectType effect : effects) {
			// draw icon
			sf::IntRect icon = Values::effects[static_cast<int>(effect)];
			buffer.quad({
				{ x, pos.y - Values::tileSize.y / 4 },
				icon.size * Values::k
			}, icon);

			// increment x
			x += icon.size.x * Values::k;
		};
		buffer.forward(&assets::interface);
	};
};