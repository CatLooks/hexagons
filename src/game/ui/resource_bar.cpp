#include "game/ui/resource_bar.hpp"

namespace gameui {
	/// Resource bar height.
	const ui::Dim Bar::height = 72px;

	/// Constructs a resource bar.
	Bar::Bar() {
		// configure bar panel
		bounds = { 0px, -height, 1ps, height };
		color = Values::dimTint;
	};

	/// Attaches a region for displaying.
	void Bar::attachRegion(const Regions::Ref& region) {
		// store region reference
		_region = &*region;

		// show bar data
		for (const auto& element : *this)
			element->activate();

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