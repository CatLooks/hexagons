#include "game/ui/resource_bar.hpp"

namespace gameui {
	/// Resource bar height.
	const ui::Dim Bar::height = 72px;

	/// Constructs a resource bar.
	Bar::Bar() {
		// configure bar panel
		bounds = { 0px, -height, 1ps, height };
		color = Values::dimTint;

		// create resource space
		_resources = new ui::Element;
		_resources->bounds = { 0.5as, 0px, 0.8ps, 1ps };
		add(_resources);

		// default icon texture maps
		static const sf::IntRect icon_textures[4] = {
			Values::coin_icon,
			Values::income_arrow[1],
			Values::berry_icon,
			Values::peach_icon
		};

		// instantiate data display
		for (int i = 0; i < Count; i++) {
			// create data label
			auto* text = new ui::Text(Values::resource_text, "param");
			text->position().y = 0.5as;
			text->pos = ui::Text::ShrinkY;
			text->autosize = true;
			_labels[i] = text;

			// create data icon
			auto* icon = new ui::Image(&assets::interface, icon_textures[i]);
			icon->position().y = 0.5as;
			_icons[i] = icon;

			// create alignment container
			auto* cont = new ui::Align;
			cont->bounds = { 0.25ps * (float)i, 0px, 0.25ps, 1ps };
			_conts[i] = cont;

			cont->add(text);
			cont->add(icon);
			_resources->add(cont);
		};

		// attach label & icon update hooks
		onRecalculate([=](const sf::Time& _) {
			// ignore if no region selected
			if (!_region) return;

			// update labels
			_labels[Money ]->param("value", std::format("{}", _region->money));
			_labels[Income]->param("value", std::format("{}", _region->income * (_region->income >= 0 ? 1 : -1)));
			_labels[Berry ]->param("value", std::format("{}", _region->berry));
			_labels[Peach ]->param("value", std::format("{}", _region->peach));

			// update income arrow
			int idx = 1;
			if (_region->income > 0) idx = 0;
			if (_region->income < 0) idx = 2;
			_icons[Income]->coords = Values::income_arrow[idx];
		});

		// initialize in hidden mode
		detachRegion();
	};

	/// Attaches a region for displaying.
	void Bar::attachRegion(const Regions::Ref& region) {
		// store region reference
		_region = &*region;

		// show bar data
		for (const auto& element : *this)
			element->activate();
		recalculate();

		// animate bar
		ui::Anim* anim = ui::AnimDim::to(&position().y, 0px, sf::seconds(0.2f));
		anim->setEasing(ui::Easings::quadOut);
		push(anim);
	};
	/// Detaches a region from displaying.
	void Bar::detachRegion() {
		// reset region reference
		_region = nullptr;

		// hide bar data
		for (const auto& element : *this)
			element->deactivate();

		// animate bar
		ui::Anim* anim = ui::AnimDim::to(&position().y, -height, sf::seconds(0.2f));
		anim->setEasing(ui::Easings::quadIn);
		push(anim);
	};
};