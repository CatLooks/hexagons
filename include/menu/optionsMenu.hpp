#pragma once
#include "ui/layer.hpp"
#include "ui/element.hpp"
#include "ui/text.hpp"
#include "ui/solid.hpp"
#include "menu/ui/menuButton.hpp"
#include <functional>

class OptionsMenu : public ui::Element {
public:
    /// Options menu callback type.
    using Action = std::function<void()>;

private:
    ui::Solid* _bg   = nullptr;  /// Background panel.
    ui::Text*  _title = nullptr; /// Menu title label.

    menuui::Button* _soundBtn = nullptr; /// Sound toggle button.
    menuui::Button* _backBtn  = nullptr; /// Back navigation button.
	menuui::Button* _langBtn = nullptr;  /// Language selection button.

	menuui::Button* _vsyncBtn = nullptr;        /// VSync toggle button.
	menuui::Button* _fullscreenBtn = nullptr;   /// Fullscreen toggle button.

    std::vector<std::string> _langKeys; 
    size_t _currentLangIdx = 0;

    bool   _soundEnabled = true; /// Current sound enabled state.
    Action _onBack;              /// Back button callback.

	bool _vsyncEnabled = true;          /// Current VSync enabled state.
	bool _fullscreenEnabled = false;    /// Current fullscreen enabled state.

public:
    /// Constructs an options menu.
    OptionsMenu();

    /// Binds back button callback.
    /// @param action Callback invoked when the back button is pressed.
    void bindBack(Action action);

protected:
    /// Draws the options menu.
    /// @param target Render buffer.
    /// @param self   Element drawing area.
    void drawSelf(ui::RenderBuffer& target, sf::IntRect self) const override;

    /// Updates sound button label text.
    void updateSoundLabel();


	/// Updates language button label text.
    void updateLanguageLabel();

	/// Updates all labels after language change.
    void refreshAllText(); 

	void updateVSyncLabel();        /// Updates VSync button label text.
	void updateFullscreenLabel();   /// Updates fullscreen button label text.

    using BoolAction = std::function<void(bool)>;

private:
    BoolAction _onVSyncToggle;
    BoolAction _onFullscreenToggle;

public:
    void bindVSync(BoolAction action);     
    void bindFullscreen(BoolAction action); 
};