#pragma once
#include "ui/element.hpp"
#include "ui/solid.hpp"
#include "ui/text.hpp"
#include "menu/text_field_open.hpp"
#include "menu/ui/menuButton.hpp"
#include <functional>
#include <string>

/// Game join menu for multiplayer.
class GameJoinMenu : public ui::Element {
public:
    /// Callback for returning to previous menu.
    using Action = std::function<void()>;
    /// Callback for joining with a code.
    using JoinAction = std::function<void(const std::string&)>;

private:
    ui::Solid* _bg = nullptr;           /// Background panel.
    ui::Text* _title = nullptr;        /// Menu title.
    ui::Text* _lblInstruction = nullptr; /// Instruction label.

    ui::TextFieldOpen* _codeField = nullptr; /// The text input field.
    ui::Text* _errorMsg = nullptr;  /// Error/Status message label.

    menuui::Button* _joinBtn = nullptr; /// Confirm join button.
    menuui::Button* _backBtn = nullptr; /// Back navigation button.

    Action _onBack;         /// Back callback.
    JoinAction _onJoin;     /// Join callback.

public:
    /// Constructs the join menu.
    GameJoinMenu();

    /// Binds back button callback.
    void bindBack(Action action);

    /// Binds join action callback.
    /// @param action Callback receiving the entered code.
    void bindJoin(JoinAction action);

    /// Sets an error or status message to display.
    /// @param msg The message to display.
    /// @param error If true, draws in red, otherwise white.
    void setStatusMessage(const std::string& msg, bool error = true);

protected:
    /// Draws the element.
    void drawSelf(ui::RenderBuffer& target, sf::IntRect self) const override;

private:
    /// Builds the central input form.
    void buildForm();
    /// Validates input and updates UI state.
    void onInputUpdate(const sf::String& string);
    /// Triggers the join action.
    void attemptJoin();

	/// Refreshes all text labels.
	void refreshAllText();
};