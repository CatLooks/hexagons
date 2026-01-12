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

    login_toggle = new ui::Pages();
    login_toggle->bounds = { 1ps-100px, 0, 100px, 100px }; 


    /// Wide button factory.
    auto makeWideButton = [&](ui::Dim y_offset, std::string text) -> menuui::Button* {
        auto* btn = new menuui::Button();
        btn->setSize({ 400px, 100px });
        btn->position() = { 0.5as, 0.5as + y_offset };
        btn->setLabel()->setRaw(text);
        return btn;
    };

    auto  makeLoginButton = [&](std::string text) -> menuui::Button* {
        auto* btn = new menuui::Button();
		btn->bounds = {0, 0, 1ps, 1ps };
        btn->setSize({ 1ps, 1ps });
        btn->setLabel()->setRaw(text);
        return btn;
		};

    /// Login button.
    _loginBtn = makeLoginButton("Login");
    _loginBtn->setCall([this]() { if (_onLogin) _onLogin(); }, nullptr, menuui::Button::Click);
    login_toggle->add(_loginBtn); /// top-right toggle, not in options list

    /// Logout button.
    _logoutBtn = makeLoginButton("Logout");
    _logoutBtn->setCall([this]() { if (_onLogout) _onLogout(); }, nullptr, menuui::Button::Click);
    login_toggle->add(_logoutBtn);

    this->add(login_toggle);
    login_toggle->show(_loginBtn); /// show login button by default

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
/// Binds login button callback.
void MainMenu::bindLogin(Action action) { _onLogin = action; }
/// Binds logout button callback.
void MainMenu::bindLogout(Action action) { _onLogout = action; }



/// Draws the main menu.
void MainMenu::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
    ui::Element::drawSelf(target, self);
}
/// Sets login state.
void MainMenu::setLoggedIn(bool loggedIn) {
        if (loggedIn) {
            login_toggle->show(_logoutBtn);
        } else {
            login_toggle->show(_loginBtn);
        }
    }
