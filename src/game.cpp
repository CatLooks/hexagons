#include "game.hpp"

void drawGame(sf::RenderWindow &window) {
  sf::CircleShape circle(50);
  circle.setFillColor(sf::Color::Red);
  circle.setPosition(sf::Vector2f(400, 300));
  window.draw(circle);
}
