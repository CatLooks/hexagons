#include "menu/ui/alertPopup.hpp"

static const ui::TextSettings k_PopupFont = {
    assets::font, 30, sf::Color::White, sf::Color::Black, 0
};

AlertPopup::AlertPopup() {
    // 1. Full screen bounds for the dimmer
    bounds = { 0, 0, 1ps, 1ps };

    // 2. Visual Dimmer (Just visual, no input logic needed now)
    _dimmer = new ui::Solid();
    _dimmer->color = sf::Color(0, 0, 0, 180);
    _dimmer->bounds = { 0, 0, 1ps, 1ps };
    add(_dimmer);

    // 3. Panel
    _panel = new ui::Solid();
    _panel->color = sf::Color(40, 40, 50);
    _panel->bounds = { 0.5ps - 300px, 0.5ps - 150px, 600px, 300px };
    add(_panel);

    // 4. Message
    _message = ui::Text::raw(k_PopupFont, "");
    _message->bounds = { 0.5ps - 280px, 0.5ps - 100px, 560px, 100px };
    _message->align = ui::Text::Center;
    add(_message);

    // 5. OK Button
    _okBtn = new menuui::Button();
    _okBtn->setSize({ 160px, 60px });
    _okBtn->position() = { 0.5ps- 80px, 0.5ps +40px};
    _okBtn->setLabel()->setRaw("OK");
    
    _okBtn->setCall([this]() { hide(); }, nullptr, menuui::Button::Click);
    add(_okBtn);

    // Start hidden
    deactivate();
}

void AlertPopup::show(const std::string& text, std::function<void()> onClose) {
    _message->setRaw(text);
    _onClose = onClose;

    // DISABLE THE BACKGROUND
    if (_bgTarget) _bgTarget->deactivate();

    // Show the popup
    activate(); 
}

void AlertPopup::hide() {
    // Hide the popup
    deactivate();

    // RE-ENABLE THE BACKGROUND
    if (_bgTarget) _bgTarget->activate();

    if (_onClose) _onClose();
}