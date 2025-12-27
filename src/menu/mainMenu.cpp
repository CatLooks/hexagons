#include "menu.hpp"
#include "assets.hpp" 
#include "ui/anim/linear.hpp"

static const ui::TextSettings title_text_settings = {
    assets::font, 80, sf::Color::White,
    sf::Color::Black, 4
};

static const sf::Color k_MenuBoxColor = sf::Color(20, 20, 25, 230);
static const sf::Color k_DimmerColor  = sf::Color(0, 0, 0, 100);

/// Constructs a main menu.
MainMenu::MainMenu() {
    bounds = { 0, 0, 1ps, 1ps };

    /// Background dimmer.
    _dimmer = new ui::Solid();
    _dimmer->color  = k_DimmerColor;
    _dimmer->bounds = { 0, 0, 1ps, 1ps };
    add(_dimmer);

    /// Menu container.
    _container = new ui::Solid();
    _container->color  = k_MenuBoxColor;
    _container->bounds = { 0, 0, 500px, 700px };
    _container->position() = { 0.5as, 0.5as };
    add(_container);

    /// Title label.
    _title = ui::Text::raw(title_text_settings, "HEXAGONS");
    _title->bounds = { 0, 60px, 1ps, 0 };
    _title->align  = ui::Text::Center;
    _title->pos    = ui::Text::Static;
    _container->add(_title);

    /// Wide button factory.
    auto makeWideButton = [&](ui::Dim y_offset, std::string text) -> menuui::Button* {
        auto* btn = new menuui::Button();
        btn->setSize({ 400px, 100px });
        btn->position() = { 0.5as, 0.5as + y_offset };
        btn->setLabel()->setRaw(text);
        return btn;
    };

    /// Start button.
    _startBtn = makeWideButton(-100px, "START GAME");
    _startBtn->setCall([this]() { if (_onStart) _onStart(); }, nullptr, menuui::Button::Click);
    _container->add(_startBtn);

    /// Join button.
    _joinBtn = makeWideButton(20px, "JOIN GAME");
    _joinBtn->setCall([this]() { if (_onJoin) _onJoin(); }, nullptr, menuui::Button::Click);
    _container->add(_joinBtn);

    /// Options button.
    _optionsBtn = makeWideButton(140px, "OPTIONS");
    _optionsBtn->setCall([this]() { if (_onOptions) _onOptions(); }, nullptr, menuui::Button::Click);
    _container->add(_optionsBtn);

    /// Exit button.
    _exitBtn = makeWideButton(260px, "EXIT");
    _exitBtn->setCall([this]() { if (_onExit) _onExit(); }, nullptr, menuui::Button::Click);
    _container->add(_exitBtn);
}

/// Binds start button callback.
void MainMenu::bindStart(Action action)   { _onStart   = action; }
/// Binds join button callback.
void MainMenu::bindJoin(Action action)    { _onJoin = action; }
/// Binds exit button callback.
void MainMenu::bindExit(Action action)    { _onExit    = action; }
/// Binds options button callback.
void MainMenu::bindOptions(Action action) { _onOptions = action; }

/// Draws the main menu.
void MainMenu::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
    ui::Element::drawSelf(target, self);
}