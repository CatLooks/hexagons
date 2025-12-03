#include "game/ui/resource_bar.hpp"

namespace gameui {
	/// Resource bar height.
	const ui::Dim ResourceBar::height = 72px;

	/// Constructs a resource bar.
	ResourceBar::ResourceBar() {
		// configure bar panel
		bounds = { 0px, 0px, 1ps, height };
		color = Values::dimTint;

		// instantiate text labels
		l_coins = new ui::Text(Values::resource_text, "param");
		l_income = new ui::Text(Values::resource_text, "param");

		// instantiate icons
		i_coins = new ui::Image(&assets::interface, Values::coin_icon);
		i_income = new ui::Image(&assets::interface, Values::income_arrow[0]);

		// layout elements inside the bar
		l_coins->bounds = { 0ps, 0px, 0.25ps, 1ps };
		l_coins->align = ui::Text::E;
		l_coins->pos = ui::Text::ShrinkY;
		add(l_coins);
		l_income->bounds = { 0.5ps, 0px, 0.25ps, 1ps };
		l_income->align = ui::Text::E;
		l_income->pos = ui::Text::ShrinkY;
		add(l_income);
		i_coins->position() = { 0.25ps, 0.5as };
		add(i_coins);
		i_income->position() = { 0.75ps, 0.5as };
		add(i_income);

		// add text hooks
		l_coins->paramHook("value", [this]() -> ui::Text::Hook {
			return _region ? std::format("{}", _region->money) : "?";
		});
		l_income->paramHook("value", [this]() -> ui::Text::Hook {
			return _region ? std::format("{}", _region->income) : "?";
		});

		// add image hook
		i_income->onUpdate([this](const sf::Time& _) {
			// update image
			int income = _region ? _region->income : 0;
			if (income > 0)
				i_income->coords = Values::income_arrow[0];
			else if (income == 0)
				i_income->coords = Values::income_arrow[1];
			else
				i_income->coords = Values::income_arrow[2];
		});

		detachRegion();
	};

	/// Attaches a region for displaying.
	void ResourceBar::attachRegion(const Regions::Ref& region) {
		_region = &*region;
		for (const auto& element : *this)
			element->activate();
	};
	/// Detaches a region from displaying.
	void ResourceBar::detachRegion() {
		_region = nullptr;
		for (const auto& element : *this)
			element->deactivate();
	};
};