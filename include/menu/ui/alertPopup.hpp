#pragma once
#include "ui/element.hpp"
#include "ui/solid.hpp"
#include "ui/text.hpp"
#include "menu/ui/menuButton.hpp"
#include "assets.hpp"

class AlertPopup : public ui::Element {
    ui::Solid* _dimmer;
    ui::Solid* _panel;
    ui::Text* _message;
    menuui::Button* _okBtn;
    std::function<void()> _onClose;
    ui::Element* _bgTarget = nullptr;

public:
    AlertPopup();
    
    void setBackground(ui::Element* bg) { _bgTarget = bg; }
    void show(const std::string& text, std::function<void()> onClose = nullptr);
    void hide();
};