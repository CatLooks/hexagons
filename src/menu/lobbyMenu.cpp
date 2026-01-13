#include "menu/lobbyMenu.hpp"
#include "assets.hpp"

/// Lobby header text settings.
static const ui::TextSettings k_LobbyHeader = { assets::font, 50, sf::Color::White, sf::Color::Black, 2 };
/// Player name text settings.
static const ui::TextSettings k_PlayerNameFont = { assets::font, 30, sf::Color::White, sf::Color::Black, 0 };

/// Constructs a lobby menu.
LobbyMenu::LobbyMenu() {
    bounds = { 0, 0, 1ps, 1ps };

    _title = ui::Text::raw(k_LobbyHeader, "");
    _title->setPath("lobby.title");
    _title->bounds = { 0, 40px, 1ps, 0 };
    _title->align = ui::Text::Center;
    add(_title);

    _roomCodeLbl = ui::Text::raw(k_PlayerNameFont, "");
    _roomCodeLbl->bounds = { 0, 100px, 1ps, 0 };
    _roomCodeLbl->align = ui::Text::Center;
    _roomCodeLbl->setColor(sf::Color::Yellow);
    _roomCodeLbl->setPath("lobby.room_id");
    _roomCodeLbl->param("id", "------");
    add(_roomCodeLbl);

    // List container sized to avoid covering buttons
    _listContainer = new ui::Element();
    _listContainer->bounds = { 0.5as, 200px, 600px, 350px };
    add(_listContainer);

    // Leave button placed at bottom-left of content area
    _leaveBtn = new menuui::Button();
    _leaveBtn->setSize({ 200px, 80px });
    _leaveBtn->position() = { 0.25as, 0.9ps };
    _leaveBtn->setLabel()->setPath("lobby.leave");
    _leaveBtn->setCall([this]() { if (_onLeave) _onLeave(); }, nullptr, menuui::Button::Click);
    add(_leaveBtn);

    // Start button placed at bottom-right of content area
    _startBtn = new menuui::Button();
    _startBtn->setSize({ 250px, 80px });
    _startBtn->position() = { 0.75as, 0.9ps };
    _startBtn->setLabel()->setPath("menu.start_game");
    _startBtn->setCall([this]() { if (_onStart) _onStart(); }, nullptr, menuui::Button::Click);
    add(_startBtn);
}

/// Updates the player list.
void LobbyMenu::updateList(const std::vector<PlayerData>& players) {
    _listContainer->clear();
    ui::Dim y = 0;
    for (const auto& p : players) {
        auto* slot = createPlayerSlot(p);
        slot->bounds = { 0, y, 1ps, 60px };
        _listContainer->add(slot);
        y += 70px;
    }
}

/// Creates a UI element for a single player slot.
ui::Element* LobbyMenu::createPlayerSlot(const PlayerData& p) {
    auto* slot = new ui::Solid();
    slot->color = sf::Color(255, 255, 255, 30);

    auto* colorBox = new ui::Solid();
    colorBox->color = p.color;
    colorBox->bounds = { 10px, 10px, 40px, 40px };
    slot->add(colorBox);

    auto* name = ui::Text::raw(k_PlayerNameFont, "");
    name->bounds = { 70px, 10px, 0, 0 };
    if (p.isHost) {
        // show host annotation via localized param
        name->setPath("lobby.player_name_host");
        name->param("name", p.name);
    } else {
        name->setPath("lobby.player_name");
        name->param("name", p.name);
    }
    slot->add(name);

    return slot;
}

/// Sets the room code label.
void LobbyMenu::setRoomCode(const std::string& code) {
    if (_roomCodeLbl) _roomCodeLbl->param("id", code);
}

/// Sets host state (enables start button for host).
void LobbyMenu::setAsHost(bool isHost) {
    if (_startBtn) _startBtn->display = isHost;
}

/// Bind start callback.
void LobbyMenu::bindStart(Action action) { _onStart = action; }
/// Bind leave callback.
void LobbyMenu::bindLeave(Action action) { _onLeave = action; }