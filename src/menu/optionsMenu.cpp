#include "menu/optionsMenu.hpp"
#include "assets.hpp"

static const ui::TextSettings title_settings = {
    assets::font, 80, sf::Color::White, sf::Color::Black, 4
};

OptionsMenu::OptionsMenu(ui::Layer* layer) : _layer(layer) {
    // Root setup
    bounds = { 0, 0, 1ps, 1ps };

    // Background
    _bg = new ui::Solid();
    _bg->color = sf::Color(20, 20, 30, 255);
    _bg->bounds = { 0, 0, 1ps, 1ps };
    add(_bg);

    // Title
    _title = ui::Text::raw(title_settings, "OPTIONS");
    _title->bounds = { 0, 100px, 1ps, 0 };
    _title->align = ui::Text::Center;
    _title->pos = ui::Text::Static;
    add(_title);

    // Sound Toggle Button (Center)
    _soundBtn = new menuui::Button();
    _soundBtn->position() = { 0.5as, 0.5as };

    _soundBtn->setLabel(); // Initialize label
    updateSoundLabel();    // Set initial text

    _soundBtn->setCall([this]() {
        // Toggle logic
        _soundEnabled = !_soundEnabled;
        updateSoundLabel();
        // Here should be call to audio system, e.g.:
        // assets::setGlobalVolume(_soundEnabled ? 100.f : 0.f);
        }, nullptr, menuui::Button::Click);

    add(_soundBtn);

    // Back Button (Bottom)
    _backBtn = new menuui::Button();
    _backBtn->position() = { 0.5as, 0.5as + 140px };

    auto* backLabel = _backBtn->setLabel();
    backLabel->setRaw("BACK");

    _backBtn->setCall([this]() {
        if (_onBack) _onBack();
        }, nullptr, menuui::Button::Click);

    add(_backBtn);

    // Attach to layer and hide by default
    _layer->add(this);
    this->activate(); // Ensure it's initialized
}

void OptionsMenu::updateSoundLabel() {
    if (_soundBtn && _soundBtn->setLabel()) {
        _soundBtn->setLabel()->setRaw(_soundEnabled ? "SOUND: ON" : "SOUND: OFF");
    }
}

void OptionsMenu::bindBack(Action action) {
    _onBack = action;
}

void OptionsMenu::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
    ui::Element::drawSelf(target, self);
}