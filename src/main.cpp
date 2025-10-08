#include "game.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
  std::cout << "SFML version: " << SFML_VERSION_MAJOR << "."
            << SFML_VERSION_MINOR << "." << SFML_VERSION_PATCH << std::endl;

  sf::RenderWindow win(sf::VideoMode({1600, 900}), "App");
  win.setVerticalSyncEnabled(true);

  while (win.isOpen()) {
    while (auto event = win.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        win.close();
      }
    }
    win.clear(sf::Color(29, 31, 37));
    drawGame(win);
    win.display();
  }
  return 0;
}
