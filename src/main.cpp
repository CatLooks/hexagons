#include <SFML/Graphics.hpp>
#include "addon.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800u, 600u }), "SFML Works!");

    std::string fontPath = std::string(RESOURCES_PATH) + "LoveDays-2v7Oe.ttf";
    sf::Font font;
    if (!font.openFromFile(fontPath))
    {
        return 404;
    }
    printer();
    sf::Text text(font, "Hello, World!");
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        window.clear();
        window.draw(text);
        window.display();
    }

    return 0;
}