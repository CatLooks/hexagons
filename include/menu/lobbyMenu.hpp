#pragma once
#include "ui/element.hpp"
#include "ui/text.hpp"
#include "ui/solid.hpp"
#include "menu/ui/menuButton.hpp"
#include <vector>
#include <string>

struct PlayerData {
    std::string name;
    sf::Color color;
    bool isHost;
    bool isReady;
};

class LobbyMenu : public ui::Element {
public:
    using Action = std::function<void()>;

    LobbyMenu();

    void updateList(const std::vector<PlayerData>& players);
    void setRoomCode(const std::string& code);

    void bindStart(Action action);
    void bindLeave(Action action);
    void setAsHost(bool isHost);

private:
    ui::Element* createPlayerSlot(const PlayerData& p);

    ui::Text* _title;
    ui::Text* _roomCodeLbl;
    ui::Element* _listContainer;
    menuui::Button* _startBtn;
    menuui::Button* _leaveBtn;

    Action _onStart;
    Action _onLeave;
};