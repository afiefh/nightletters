#include "tree.hpp"

const float windWidth = 200;
int main()
{
  bool update = false;
  sf::RectangleShape rectangle(sf::Vector2f(200, 200));
  rectangle.setFillColor(sf::Color(133,133,133,50));
  srand(time(NULL));
  sf::RenderWindow window(sf::VideoMode(800, 600), "Nightletters");
  window.setFramerateLimit(60);
  
  SoftBody softBody("../graphic/tree_leaves.png");
  softBody.move(20, 20);
  float windLeft = -windWidth;
  rectangle.setPosition(-windWidth,0);
  //softBody.pushRight(500, 0);
  while (window.isOpen())
  {
    window.clear(sf::Color::White);
    // Event processing
    sf::Event event;
    while (window.pollEvent(event))
    {
      // Request for closing the window
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {
        update = true;
      } else if (event.type == sf::Event::KeyReleased) {
        update = false;
      }
    }
    if (update) {
      softBody.velocityRight(windLeft, windLeft+windWidth, 0, 200, 10);
      windLeft += 1;
      rectangle.move(1,0);
    }
    softBody.update();
    window.draw(softBody);
    if (update) window.draw(rectangle);
    
    // End the current frame and display its contents on screen
    window.display();
  }
}
