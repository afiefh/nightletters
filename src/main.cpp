
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

#include "Nightsky.hpp"
#include "StrokedText.hpp"
#include <cwchar>
#include <iostream>
#include <cassert>
#include <stdio.h>
#include <cmath>

using namespace std;

int main() 
{
  sf::RenderWindow window(sf::VideoMode(800, 600), "Nightletters");
  window.setFramerateLimit(60);

  Nightsky nightsky(sf::Vector2i(800, 600));
 
  //text stuff
  sf::Font mf;
  mf.loadFromFile("amiri-regular.ttf");
  sf::String s;
  StrokedText text;
  text.setCharacterSize (50);
  text.setColor(sf::Color::Black, sf::Color(100,100,100));
  text.setPosition(sf::Vector2f(50,50));
  text.setString(s);
  text.setFont(mf);
  
  // The main loop - ends as soon as the window is closed
  while (window.isOpen())
  {
    if (s == sf::String(L"عفيف")) {
      s.clear();
      text.setString(s);
    }
    // Event processing
    sf::Event event;
    while (window.pollEvent(event))
    {
	// Request for closing the window
	if (event.type == sf::Event::Closed) {
	    window.close();
        } else if (event.type == sf::Event::TextEntered) {
          if (event.text.unicode == '\b') {
            if (s.getSize() <= 0) continue;
            s.erase(s.getSize() - 1, 1);
          } else {
            s += event.text.unicode;
          }
          text.setString(s);
        }
    }
    
    nightsky.update();
    
    // Clear the whole window before rendering a new frame
    window.clear(sf::Color(255,255,255));
    
    // Draw some graphical entities
    window.draw(nightsky);
    window.draw(text);
    
    // End the current frame and display its contents on screen
    window.display();
  }
}
