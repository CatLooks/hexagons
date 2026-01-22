#include "menu/gameJoinMenu.hpp"
#include "assets.hpp"
#include "menu/text_field_open.hpp"
#include <SFML/System/String.hpp>
#include <string>

/// Title text settings.
static const ui::TextSettings k_TitleFont = {
    assets::font, 60, sf::Color::White, sf::Color::Black, 3
};
/// Label text settings.
static const ui::TextSettings k_LabelFont = {
    assets::font, 30, sf::Color(200, 200, 200), sf::Color::Black, 1
};
/// Input text settings.
static const ui::TextSettings k_InputFont = {
    assets::font, 40, sf::Color::White, sf::Color::Transparent, 0
};
/// Error text settings.
static const ui::TextSettings k_ErrorFont = {
    assets::font, 25, sf::Color::Red, sf::Color::Black, 1
};

/// Helper to create a basic panel map for the text field background.
static ui::Panel::Map makeInputPanelMap() {
    return ui::Panel::Map::rect(&assets::interface, Values::coords(0, 0), { 6, 6 }, 2);
}

/// Constructs a game join menu.
GameJoinMenu::GameJoinMenu(Net* net) : _net(net) {
    bounds = { 0, 0, 1ps, 1ps };

    /// Background.
    _bg = new ui::Solid();
    _bg->color = sf::Color(10, 10, 15, 240);
    _bg->bounds = { 0, 0, 1ps, 1ps };
    add(_bg);

    /// Title.
    _title = ui::Text::raw(k_TitleFont, "Join Game");
    _title->bounds = { 0, 0.15ps, 1ps, 0 };
    _title->align = ui::Text::Center;
    _title->pos = ui::Text::Static;
    add(_title);

    /// Form container.
    buildForm();

    /// Back button.
    _backBtn = new menuui::Button();
    _backBtn->setSize({ 180px, 80px });
    _backBtn->position() = { 50px, 1ps - 100px };
    _backBtn->setLabel()->setRaw("BACK");
    _backBtn->setCall([this]() {
        if (_onBack) _onBack();
    }, nullptr, menuui::Button::Click);
    add(_backBtn);

    /// Join button.
    _joinBtn = new menuui::Button();
    _joinBtn->setSize({ 180px, 80px });
    _joinBtn->position() = { 1ps - 230px, 1ps - 100px };
    _joinBtn->setLabel()->setRaw("JOIN");
    _joinBtn->setCall([this]() { attemptJoin(); }, nullptr, menuui::Button::Click);
    _joinBtn->setLabel()->setColor(sf::Color(100, 100, 100));
    add(_joinBtn);

    assets::lang::refresh_listeners.push_back([this]() { refreshAllText(); });
    refreshAllText();
}

/// Builds input form.
void GameJoinMenu::buildForm() {
    /// Instruction label.
    _lblInstruction = ui::Text::raw(k_LabelFont, "Enter 6-digit Room Code:");
    _lblInstruction->bounds = { 0, 0.4ps - 40px, 1ps, 0 };
    _lblInstruction->align = ui::Text::Center;
    _lblInstruction->pos = ui::Text::Static;
    add(_lblInstruction);

    /// Text field.
    _codeField = new ui::TextFieldOpen(makeInputPanelMap(), k_InputFont, sf::Color::White);
    _codeField->bounds = { 0.5ps - 200px, 0.4ps, 400px, 60px };
    _codeField->padding = { 10, 10, 10, 10 };

    /// Input validation: digits only, max length 6.
    _codeField->input.attachValidation([](const sf::String& str, char32_t c) {
        if (c == '\b') return true;
        if (str.getSize() > 6) return false;
        return (c >= '0' && c <= '9');
    });

    /// Cursor focus: toggle join availability.
    _codeField->input.attachCursorFocus([this](unsigned int cursor) {
        if (!_errorMsg->string().isEmpty()) _errorMsg->setRaw("");
        if (cursor == 6) {
            _joinBtn->setLabel()->setColor(sf::Color::White);
            _joinBtn->setCall([this]() { attemptJoin(); }, nullptr, menuui::Button::Click);
        } else {
            _joinBtn->setLabel()->setColor(sf::Color(100, 100, 100));
            _joinBtn->setCall(nullptr, nullptr, menuui::Button::Click);
        }
        _codeField->setCursorVisual(cursor);
    });

    /// Text confirm: handle enter key.
    _codeField->input.attachTextConfirm([this](const sf::String&) {
        attemptJoin();
    });


    _codeField->focus(true);
    add(_codeField);

    /// Error/status message.
    _errorMsg = ui::Text::raw(k_ErrorFont, "");
    _errorMsg->bounds = { 0, 0.4ps + 80px, 1ps, 0 };
    _errorMsg->align = ui::Text::Center;
    _errorMsg->pos = ui::Text::Static;
    add(_errorMsg);
}

/// Input update handler.
void GameJoinMenu::onInputUpdate(const sf::String& string) {
    if (!_errorMsg->string().isEmpty()) {
        _errorMsg->setRaw("");
    }
}

/// Attempts to join a game.
void GameJoinMenu::attemptJoin() {
    std::string code;
    if (auto open = dynamic_cast<ui::TextFieldOpen*>(_codeField)) {
        code = open->text().toAnsiString();
    }

    if (code.length() != 6) {
        setStatusMessage("Invalid Code Length", true);
        return;
    }

    setStatusMessage("Connecting...", false);

    // Clear any old handlers
    _net->clearHandlers();

    // Bind Success Handler
    _net->OnLobbySuccess.add([this, code]() {
        // Unbind self (GameStartMenu will take over)
        //_net->clearHandlers(); 
        
        // Trigger the screen switch
        if (_onJoinSuccess) _onJoinSuccess(code);
    });

    // 3. Bind Failure Handler (idk, trza pewnie dodaæ...)
    /* 
    _net->OnJoinFailed.add([this](const std::string& error) {
        setStatusMessage("Error: " + error, true);
        _joinBtn->setLabel()->setColor(sf::Color::White); // Re-enable button
    });
    */

    // Start Connection
    _net->connect(code);
}

void GameJoinMenu::bindJoinSuccess(JoinSuccessAction action) {
    _onJoinSuccess = action;
}

/// Sets status message.
void GameJoinMenu::setStatusMessage(const std::string& msg, bool error) {
    _errorMsg->setRaw(msg);
    _errorMsg->setColor(error ? sf::Color::Red : sf::Color::White);
}

/// Binds back button callback.
void GameJoinMenu::bindBack(Action action) {
    _onBack = action;
}

/// Binds join button callback.
void GameJoinMenu::bindJoin(JoinAction action) {
    _onJoin = action;
}

/// Draws the menu.
void GameJoinMenu::drawSelf(ui::RenderBuffer& target, sf::IntRect self) const {
    ui::Element::drawSelf(target, self);
}

/// Refreshes all text labels.
void GameJoinMenu::refreshAllText() {
    _title->setPath("join.title");
    _lblInstruction->setPath("join.instruction");
    _backBtn->setLabel()->setPath("menu.back");
    _joinBtn->setLabel()->setPath("menu.join");
}

void GameJoinMenu::reset() {
    // 1. Clear the text field and grab focus
    if (_codeField) {
		_codeField->setText("");
        _codeField->input.clear();
        _codeField->focus(true);
    }

    // 2. Clear any old status/error messages
    setStatusMessage("", false);

    // 3. Reset the Join button to its "Disabled" state
    // (We mimic the logic from attachCursorFocus here: grey color, no callback)
    if (_joinBtn) {
        _joinBtn->setLabel()->setColor(sf::Color(100, 100, 100));
        _joinBtn->setCall(nullptr, nullptr, menuui::Button::Click);
    }

    // 4. Critical: Stop listening for network events
    // If the user backed out while "Connecting...", we don't want 
    // a delayed success packet to suddenly switch screens later.
    if (_net) {
        _net->clearHandlers();
    }
}
