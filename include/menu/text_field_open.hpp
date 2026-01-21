#pragma once

#include "ui/textfield.hpp"

namespace ui {
    /// Thin subclass exposing internal text label and string of `ui::TextField`.
    class TextFieldOpen : public TextField {
    public:
        using TextField::TextField;

        /// Returns pointer to the internal label element.
        ui::Text* label() { return _label; }

        /// Returns current raw string of the text field.
        const sf::String& text() const { return _label->string(); }

        /// Set raw string of the text field.
        void setText(const sf::String& s) { _label->setRaw(s); }
    };
};