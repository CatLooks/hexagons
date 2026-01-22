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
    _container->bounds = { 0, 0, 500px, 820px };
    _container->position() = { 0.5as, 0.5as };
    add(_container);

    /// Title label.
    _title = new ui::Text(title_text_settings, "menu.title");
    _title->bounds = { 0, 60px, 1ps, 0 };
    _title->align  = ui::Text::Center;
    _title->pos    = ui::Text::Static;
    _container->add(_title);

    login_toggle = new ui::Pages();
    login_toggle->bounds = { 1as, 0px, 100px, 100px }; 


    /// Wide button factory.
    auto makeWideButton = [&](ui::Dim y_offset, const localization::Path& text) -> menuui::Button* {
        auto* btn = new menuui::Button();
        btn->setSize({ 400px, 100px });
        btn->position() = { 0.5as, 0.5as + y_offset };
        btn->setLabel()->setPath(text);
        return btn;
    };

    auto  makeLoginButton = [&](const localization::Path& text) -> menuui::Button* {
        auto* btn = new menuui::Button();
		btn->bounds = {0, 0, 1ps, 1ps };
        btn->setSize({ 1ps, 1ps });
        btn->setLabel()->setPath(text);
        return btn;
		};

    /// Login button.
    _loginBtn = makeLoginButton("menu.login");
    _loginBtn->setCall([this]() { if (_onLogin) _onLogin(); }, nullptr, menuui::Button::Click);
    login_toggle->add(_loginBtn); /// top-right toggle, not in options list

    /// Logout button.
    _logoutBtn = makeLoginButton("menu.logout");
    _logoutBtn->setCall([this]() { if (_onLogout) _onLogout(); }, nullptr, menuui::Button::Click);
    login_toggle->add(_logoutBtn);

    this->add(login_toggle);
    login_toggle->show(_loginBtn); /// show login button by default

    /// Start button.
    _startBtn = makeWideButton(-160px, "menu.start");
    _startBtn->setCall([this]() { if (_onStart) _onStart(); }, nullptr, menuui::Button::Click);
    _container->add(_startBtn);

    /// Join button.
    _joinBtn = makeWideButton(-40px, "menu.join");
    _joinBtn->setCall([this]() { if (_onJoin) _onJoin(); }, nullptr, menuui::Button::Click);
    _container->add(_joinBtn);
    
    /// Editor button.
    _editBtn = makeWideButton(80px, "menu.editor");
    _editBtn->setCall([this]() { if (_onEdit) _onEdit(); }, nullptr, menuui::Button::Click);
    _container->add(_editBtn);

    /// Options button.
    _optionsBtn = makeWideButton(200px, "menu.options");
    _optionsBtn->setCall([this]() { if (_onOptions) _onOptions(); }, nullptr, menuui::Button::Click);
    _container->add(_optionsBtn);

    /// Exit button.
    _exitBtn = makeWideButton(320px, "menu.exit");
    _exitBtn->setCall([this]() { if (_onExit) _onExit(); }, nullptr, menuui::Button::Click);
    _container->add(_exitBtn);

   assets::lang::refresh_listeners.push_back([this]() { refreshAllText(); });

    refreshAllText();
}

/// Binds start button callback.
void MainMenu::bindStart(Action action)   { _onStart   = action; }
/// Binds join button callback.
void MainMenu::bindJoin(Action action)    { _onJoin = action; }
/// Binds exit button callback.
void MainMenu::bindExit(Action action)    { _onExit    = action; }
/// Binds editor button callback.
void MainMenu::bindEdit(Action action) { _onEdit = action; }
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


/// Updates all labels after language change.
void MainMenu::refreshAllText() {
    _title->setPath("menu.title");
    _startBtn->setLabel()->setPath("menu.start");
    _joinBtn->setLabel()->setPath("menu.join");
    _optionsBtn->setLabel()->setPath("menu.options");
    _exitBtn->setLabel()->setPath("menu.exit");
    _loginBtn->setLabel()->setPath("menu.login");
    _logoutBtn->setLabel()->setPath("menu.logout");
}