#include "game/ui/splash_text.hpp"

namespace gameui {
	/// Splash background panel map.
	const ui::Panel::Map Splash::texture = ui::Panel::Map::rect
		(&assets::interface, Values::coords(3, 0), { 4, 8 }, 2);

	/// Constructs an empty splash text element.
	Splash::Splash(ui::Dim height, ui::Dim text): ui::Panel(texture), _offset(text) {
		// set splash size
		bounds = { 0px, 0.5as, 1ps, height };
		scissor = true;
		infinite = true;

		// reset text queue
		frame();

		// mask events
		onEvent([=](const ui::Event& evt) {
			// ignore if not masking
			if (!_inhibit) return false;

			// mask LMB
			if (auto data = evt.get<ui::Event::MousePress>())
				if (data->button == sf::Mouse::Button::Left)
					return true;
			return false;
		});

		// deactivate by default
		deactivate();
	};

	/// Queues a text label for display.
	void Splash::queue(const localization::Path& path, sf::Color color) {
		// create text element
		ui::Text* text = new ui::Text(Values::splash_text, path);
		text->pos = ui::Text::Static;
		text->autosize = true;
		text->position().y = _offset;
		text->use(&args);
		text->setColor(color);

		// add text to queue
		_text.back().push_back(std::unique_ptr<ui::Text>(text));
	};

	/// Creates a new splash frame.
	void Splash::frame() {
		_text.push_back({});
	};

	/// Transparent white.
	static const sf::Color color_out = sf::Color(255, 255, 255, 0);
	/// Opaque white.
	static const sf::Color color_in = sf::Color(255, 255, 255, 255);

	/// Displays queued text labels.
	void Splash::display() {
		// delete any existing containers
		clear();
		cancel();

		// activate panel
		activate();
		color.a = 0;
		
		// start blocking mouse events
		_inhibit = true;

		// create text aligning containers
		std::vector<ui::Align*> conts;
		for (auto& group : _text) {
			// ignore if empty
			if (group.empty()) continue;

			// create container
			ui::Align* align = new ui::Align;
			align->position().y = -1ps;
			add(align);
			conts.push_back(align);

			// add text labels to container
			for (auto& text : group)
				align->add(text.release());
		};

		// add animations to containers
		for (size_t i = 0; i < conts.size(); i++) {
			auto* prev = conts[i];
			auto* next = i + 1 < conts.size() ? conts[i + 1] : nullptr;

			// create entrance animation
			auto* anim = new ui::AnimDim(&prev->position().y, -1ps, 0ps, sf::seconds(0.25f));
			anim->ease = ui::Easings::quadOut;
			anim->setAfter([=]() {
				// wait before exiting
				push(new ui::AnimTimer(sf::seconds(1.4f), [=]() {
					// activate next container
					if (next) next->activate();

					// create exit animation
					auto* anim = new ui::AnimDim(&prev->position().y, 0ps, 1ps, sf::seconds(0.25f));
					anim->ease = ui::Easings::quadIn;
					anim->setAfter([=]() {
						// deactivate container
						prev->deactivate();

						// fade out panel
						if (!next) fade();
					});
					push(anim);
				}));
			});
			prev->push(anim);
			prev->deactivate();
		};

		// reset text queue
		_text.clear();
		frame();

		// create fade in animation
		auto* anim = new ui::AnimColor(&color, color_out, color_in, sf::seconds(0.16f));
		anim->setAfter([=]() {
			// enable first container
			if (!children().empty())
				children().front()->activate();
			else fade();
		});
		push(anim);
	};

	/// Fades out the panel.
	void Splash::fade() {
		// stop blocking mouse events
		_inhibit = false;

		// create fade out animation
		auto* anim = new ui::AnimColor(&color, color_in, color_out, sf::seconds(0.16f));
		anim->setAfter([=]() {
			// deactivate panel
			deactivate();
		});
		push(anim);
	};
};