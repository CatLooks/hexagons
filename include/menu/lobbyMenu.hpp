#pragma once

// include dependencies
#include "ui/element.hpp"
#include "ui/text.hpp"
#include "ui/solid.hpp"
#include "menu/ui/menuButton.hpp"
#include <vector>
#include <string>
#include <functional>

/// Player data structure for lobby list.
struct PlayerData {
    std::string name;  /// Player display name.
    sf::Color color;   /// Player color.
    bool isHost;       /// Whether the player is host.
    bool isReady;      /// Whether the player is ready.
};

/// Lobby menu element.
class LobbyMenu : public ui::Element {
public:
    /// Action callback type.
    using Action = std::function<void()>;

    /// Constructs a lobby menu.
    LobbyMenu();

    /// Updates player list display.
    void updateList(const std::vector<PlayerData>& players);
    /// Sets the displayed room code.
    void setRoomCode(const std::string& code);

    /// Binds start button callback.
    void bindStart(Action action);
    /// Binds leave button callback.
    void bindLeave(Action action);
    /// Sets host state (enables start button if host).
    void setAsHost(bool isHost);

    /// Refreshes all localized text (called on language change).
    void refreshText();

private:
    /// Creates a UI element representing a single player slot.
    ui::Element* createPlayerSlot(const PlayerData& p);

    ui::Text* _title;           /// Menu title.
    ui::Text* _roomCodeLbl;     /// Room code label.
    ui::Element* _listContainer; /// Container for player slots.
    menuui::Button* _startBtn;  /// Start game button.
    menuui::Button* _leaveBtn;  /// Leave room button.

    Action _onStart;            /// Start callback.
    Action _onLeave;            /// Leave callback.

    // Stored state for localization refresh
    std::vector<PlayerData> _players; /// Last known player list.
    std::string _roomCode;            /// Last known room code.
};