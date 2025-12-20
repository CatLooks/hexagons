#include "menu/optionsMenu.hpp"
#include "assets.hpp"

static const ui::TextSettings title_settings = {
    assets::font, 80, sf::Color::White, sf::Color::Black, 4
};

/// Constructs an options menu.
OptionsMenu::OptionsMenu() {
	// set root bounds
    bounds = { 0, 0, 1ps, 1ps };

	// background panel
    _bg = new ui::Solid();
    _bg->color  = sf::Color(20, 20, 30, 255);
    _bg->bounds = { 0, 0, 1ps, 1ps };
    add(_bg);

	// title label
    _title = ui::Text::raw(title_settings, "OPTIONS");
    _title->bounds = { 0, 100px, 1ps, 0 };
    _title->align  = ui::Text::Center;
    _title->pos    = ui::Text::Static;
    add(_title);

	// sound toggle button
    _soundBtn = new menuui::Button();
    _soundBtn->setSize({ 175px, 125px });
    _soundBtn->position() = { 0.5as, 0.5as };
    _soundBtn->setLabel();
    updateSoundLabel();
    _soundBtn->setCall([this]() {
        _soundEnabled = !_soundEnabled;
        updateSoundLabel();
    }, nullptr, menuui::Button::Click);
    add(_soundBtn);

	// back button
    auto makeWideButton = [&](ui::Dim y_offset, std::string text) -> menuui::Button* {
        auto* btn = new menuui::Button();
        btn->setSize({ 400px, 100px });
        btn->position() = { 0.5as, 0.5as + y_offset };
        btn->setLabel()->setRaw(text);
        return btn;
    };

    _backBtn = makeWideButton(140px, "BACK");
    _backBtn->setCall([this]() {
        if (_onBack) _onBack();
    }, nullptr, menuui::Button::Click);
    add(_backBtn);
}

/// Updates sound button label text.
void OptionsMenu::updateSoundLabel() {
    if (_soundBtn && _soundBtn->setLabel()) {
        _soundBtn->setLabel()->setRaw(_soundEnabled ? "SOUND: ON" : "SOUND: OFF");
    }
}

/// Binds back button callback.
void OptionsMenu::bindBack(Action action) {
    _onBack = action;
}

/// Draws the options menu.
void OptionsMenu::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
    ui::Element::drawSelf(target, self);
}