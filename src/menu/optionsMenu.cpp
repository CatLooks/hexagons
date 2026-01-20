#include "menu/optionsMenu.hpp"
#include "assets.hpp"

static const ui::TextSettings title_settings = {
    assets::font, 80, sf::Color::White, sf::Color::Black, 4
};

static const ui::DimVector BUTTON_SIZE = { 400px, 80px };

/// Constructs an options menu.
OptionsMenu::OptionsMenu() {
    bounds = { 0, 0, 1ps, 1ps };

    // 1. Load Language List and find English index
    assets::loadLanguageList(); 
    _currentLangIdx = 0; // Default fallback
    
    for (auto const& [key, val] : assets::lang::index) {
        _langKeys.push_back(key);
        // Check if this is the standard language (e.g., "en")
        if (key == assets::lang::standard || key == "en") {
            _currentLangIdx = _langKeys.size() - 1;
        }
    }

    // Load the default language file immediately
    if (!_langKeys.empty()) {
        assets::loadLanguage(assets::lang::index[_langKeys[_currentLangIdx]].file);
    }

    /// Background panel.
    _bg = new ui::Solid();
    _bg->color  = sf::Color(20, 20, 30, 255);
    _bg->bounds = { 0, 0, 1ps, 1ps };
    add(_bg);

    /// Title label.
    _title = new ui::Text(title_settings, "menu.options");
    _title->bounds = { 0, 100px, 1ps, 0 };
    _title->align  = ui::Text::Center;
    add(_title);

    _vsyncBtn = new menuui::Button();
    _vsyncBtn->setSize(BUTTON_SIZE);
    _vsyncBtn->position() = { 0.5as, 0.5as - 200px };
    _vsyncBtn->setCall([this]() {
        _vsyncEnabled = !_vsyncEnabled;
        updateVSyncLabel();
        if (_onVSyncToggle) _onVSyncToggle(_vsyncEnabled);
    }, nullptr, menuui::Button::Click);
    add(_vsyncBtn);

    // Fullscreen Button
    _fullscreenBtn = new menuui::Button();
    _fullscreenBtn->setSize(BUTTON_SIZE);
    _fullscreenBtn->position() = { 0.5as, 0.5as - 100px };
    _fullscreenBtn->setCall([this]() {
        _fullscreenEnabled = !_fullscreenEnabled;
        updateFullscreenLabel();
        if (_onFullscreenToggle) _onFullscreenToggle(_fullscreenEnabled);
    }, nullptr, menuui::Button::Click);
    add(_fullscreenBtn);

    /// Sound toggle button.
    _soundBtn = new menuui::Button();
    _soundBtn->setSize(BUTTON_SIZE);
    _soundBtn->position() = { 0.5as, 0.5as };
    _soundBtn->setLabel();
    _soundBtn->setCall([this]() {
        _soundEnabled = !_soundEnabled;
        updateSoundLabel();
    }, nullptr, menuui::Button::Click);
    add(_soundBtn);
  
    /// Language selection button.
    _langBtn = new menuui::Button();
    _langBtn->setSize(BUTTON_SIZE);
    _langBtn->position() = { 0.5as, 0.5as + 100px };
    _langBtn->setLabel();
    _langBtn->setCall([this]() {
        assets::lang::next();
        refreshAllText();
    }, nullptr, menuui::Button::Click);
    add(_langBtn);

    /// Back button.
    _backBtn = new menuui::Button();
    _backBtn->setSize(BUTTON_SIZE);
    _backBtn->position() = { 0.5as, 0.5as + 200px };
    _backBtn->setLabel()->setPath("menu.back");
    _backBtn->setCall([this]() { if (_onBack) _onBack(); }, nullptr, menuui::Button::Click);
    add(_backBtn);

    // Initial fill of all text
    refreshAllText();
}


/// Updates sound button label text.
void OptionsMenu::updateSoundLabel() {
    if (_soundBtn && _soundBtn->setLabel()) {
        // We use parameters as defined in localization.md
        ui::Text* lbl = _soundBtn->setLabel();
        lbl->setPath("menu.sound");
        // Import "ON" or "OFF" strings from the locale
        lbl->param("v", _soundEnabled ? "@!menu.on" : "@!menu.off");
    }
}


/// Updates language button label text.
void OptionsMenu::updateLanguageLabel() {
    if (_langBtn && _langBtn->setLabel()) {
        ui::Text* lbl = _langBtn->setLabel();
        lbl->setPath("menu.lang");
        // Get the name directly from the global assets
        lbl->param("v", assets::lang::current_name()); 
    }
}


/// Updates all labels after language change.
void OptionsMenu::refreshAllText() {
    updateSoundLabel();
    updateLanguageLabel();
    updateVSyncLabel();
	updateFullscreenLabel();

    _title->setPath("menu.options");
    _backBtn->setLabel()->setPath("menu.back");
}


/// Binds back button callback.
void OptionsMenu::bindBack(Action action) {
    _onBack = action;
}

/// Draws the options menu.
void OptionsMenu::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
    ui::Element::drawSelf(target, self);
}

void OptionsMenu::updateVSyncLabel() {
    if (_vsyncBtn && _vsyncBtn->setLabel()) {
        ui::Text* lbl = _vsyncBtn->setLabel();
        lbl->setPath("menu.vsync"); // "menu.vsync": "VSync: {v}" 
        lbl->param("v", _vsyncEnabled ? "@!menu.on" : "@!menu.off");
    }
}

void OptionsMenu::updateFullscreenLabel() {
    if (_fullscreenBtn && _fullscreenBtn->setLabel()) {
        ui::Text* lbl = _fullscreenBtn->setLabel();
        lbl->setPath("menu.fullscreen"); // "menu.fullscreen": "Display: {v}"
        lbl->param("v", _fullscreenEnabled  ? "@!menu.on" : "@!menu.off");
    }
}

void OptionsMenu::bindVSync(BoolAction action) { _onVSyncToggle = action; }
void OptionsMenu::bindFullscreen(BoolAction action) { _onFullscreenToggle = action; }

