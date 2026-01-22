#include "ui/exit.hpp"
#include "assets.hpp"

namespace ui {
	/// Constructs an exit element.
	Exit::Exit(StaticHandler callback, ui::Dim y, sf::Keyboard::Key key):
		ui::Text({ assets::font, 24, sf::Color::White, sf::Color::Black, 2.f }, "exit")
	{
		// set text position
		position() = { 8px, y + 8px };

		// add exit callback
		onUpdate([=](const sf::Time& delta) {
			// update timer
			if (sf::Keyboard::isKeyPressed(key))
				_time += (float)delta.asSeconds();
			else
				_time = 0.f;

			// invoke callback
			if (_time >= 2.f) {
				_time = 0.f;
				callback();
			};

			// get opacity
			uint8_t opacity = _time >= 1.f ? 255 : _time * 255;

			// update visibility
			setColor(sf::Color(255, 255, 255, opacity));
			setOutline(sf::Color(0, 0, 0, opacity), 2.f);
		});
	};
};