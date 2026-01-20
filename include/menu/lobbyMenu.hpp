#pragma once

// include dependencies
#include "ui/element.hpp"
#include "ui/text.hpp"
#include "ui/solid.hpp"
#include "menu/ui/menuButton.hpp"
#include <vector>
#include <string>
#include <functional>

//Structures for handling data transfers (???)

/// Map information structure.
struct MapInfo {
    std::string id;                         // Unique ID (filename for now)
    std::string displayName;                // Localized name or name from file
    std::string filePath;                   // Path to the .json map file
    std::string previewPath;                // Path to the preview image
	std::shared_ptr<sf::Texture> texture;   // Loaded texture for preview
};

/// Player data structure for lobby list.
struct PlayerData {
    std::string name;           /// Player display name.
    sf::Color color;            /// Player color.
    bool isHost = false;        /// Whether the player is host.
    bool isReady = false;       /// Whether the player is ready.
};

/// Game data structure for lobby details.
struct GameData {
	bool isMultiplayer = false;      /// Whether the game is multiplayer.
    std::string roomCode;            /// Room code.
    std::vector<PlayerData> players; /// List of players in the lobby.
    
    enum class Difficulty {
        Easy,
        Medium,
        Hard
	} difficulty = Difficulty::Easy; /// Selected difficulty
    
     std::string selectedMapPath;    /// Store the path to the selected map file
     std::string selectedMapName;    /// Store for easy UI display

	unsigned int maxPlayers = 2;     /// Maximum number of players.   
};;


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

	/// Sets game details (room code, player list, etc).
     void setGameDetails(const GameData& data);
     /// Sets the interactable state of the menu
     void setInteractable(bool enabled);

private:
    /// Creates a UI element representing a single player slot.
    ui::Element* createPlayerSlot(const PlayerData& p);

    ui::Text* _title;           /// Menu title.
    ui::Text* _roomCodeLbl;     /// Room code label.
    ui::Element* _listContainer;/// Container for player slots.
    menuui::Button* _startBtn;  /// Start game button.
    menuui::Button* _leaveBtn;  /// Leave room button.

    Action _onStart;            /// Start callback.
    Action _onLeave;            /// Leave callback.

    // Stored state for localization refresh
    std::vector<PlayerData> _players; /// Last known player list.
    std::string _roomCode;            /// Last known room code.

	ui::Text* _detailsLbl;            /// Game details label.
	GameData _lastData; 		      /// Last known game data.
};