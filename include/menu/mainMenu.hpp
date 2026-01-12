#pragma once
#include "ui/layer.hpp"
#include "ui/element.hpp"
#include "ui/text.hpp"
#include "ui/solid.hpp"
#include "menu/ui/menuButton.hpp"
#include <functional>

class MainMenu : public ui::Element {
public:
    /// Main menu callback type.
    using Action = std::function<void()>;

	ui::Pages*     login_toggle = nullptr; /// Login page container.
    menuui::Button* _loginBtn   = nullptr; /// "Login" button.
    menuui::Button* _logoutBtn   = nullptr; /// "Logout" button.


   
	/// Sets which login or logout button is visible.
	/// @param loggedIn Whether the user is logged in.
    void setLoggedIn(bool loggedIn);

private:
    ui::Solid* _bg        = nullptr; /// Optional background panel.
    ui::Text*  _title     = nullptr; /// Menu title label.
    ui::Solid* _dimmer    = nullptr; /// Fullscreen dimmer.
    ui::Solid* _container = nullptr; /// Central menu box.
    ui::Image* _bgImage   = nullptr; /// Optional background image.

    menuui::Button* _startBtn   = nullptr; /// "Start game" button.
    menuui::Button* _joinBtn    = nullptr; /// "Join game" button.
    menuui::Button* _optionsBtn = nullptr; /// "Options" button.
    menuui::Button* _exitBtn    = nullptr; /// "Exit" button.


    Action _onStart;   /// Start callback.
    Action _onJoin;    /// Join callback.
    Action _onExit;    /// Exit callback.
    Action _onOptions; /// Options callback.
    Action _onLogin;   /// Login callback.
	Action _onLogout;  /// Logout callback.
public:
    /// Constructs a main menu.
    MainMenu();

    /// Binds start button callback.
    /// @param action Callback invoked when the start button is pressed.
    void bindStart(Action action);
    /// Binds join button callback.
    /// @param action Callback invoked when the join button is pressed.
    void bindJoin(Action action);
    /// Binds exit button callback.
    /// @param action Callback invoked when the exit button is pressed.
    void bindExit(Action action);
    /// Binds options button callback.
    /// @param action Callback invoked when the options button is pressed.
    void bindOptions(Action action);
    /// Binds options button callback.
    /// @param action Callback invoked when the login button is pressed.
    void bindLogin(Action action);
    /// Binds options button callback.
    /// @param action Callback invoked when the login button is pressed.
    void bindLogout(Action action);


protected:
    /// Draws the main menu.
    /// @param target Render buffer.
    /// @param self   Element drawing area.
    void drawSelf(ui::RenderBuffer& target, sf::IntRect self) const override;
};