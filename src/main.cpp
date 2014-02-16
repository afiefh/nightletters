
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

#include "ComplexText.hpp"
#include "bezier.hpp"
#include "Breeze.hpp"
#include "Moon.hpp"
#include "Starfield.hpp"
#include <cwchar>
#include <iostream>
#include <cassert>
#include <stdio.h>
#include <cmath>

using namespace std;

class Nightsky : public sf::Drawable{
public:
  Nightsky(const sf::Vector2i& windowSize) : m_moon("../graphic/moon.png", windowSize), m_starfield(150, "../graphic/star.png", windowSize), m_breeze(windowSize, "../graphic/wind.png", 3, 2, 0.2, 0.4)
  {
    m_backgroundTex.loadFromFile("../graphic/hill_background.png");
    m_foregroundTex.loadFromFile("../graphic/hill_foreground.png");
    
    m_background.setTexture (m_backgroundTex, true);
    m_foreground.setTexture (m_foregroundTex, true);
  }
  
  void update() 
  {
    m_breeze.update();
    m_moon.updatePosition();
    m_starfield.update();
  }
  
  virtual void  draw (sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(m_background, states);
    target.draw(m_starfield, states);
    target.draw(m_moon, states);
    target.draw(m_foreground, states);
    target.draw(m_breeze, states);
  }
  
private:
  sf::Texture m_backgroundTex;
  sf::Texture m_foregroundTex;
  
  sf::Sprite m_background;
  sf::Sprite m_foreground;

  Moon m_moon;
  Starfield m_starfield;
  Breeze m_breeze;
};

int main() 
{
  sf::RenderWindow window(sf::VideoMode(800, 600), "Nightletters");
  window.setFramerateLimit(60);

  
  sf::Texture backgroundTex;
  backgroundTex.loadFromFile("../graphic/hill_background.png");
  sf::Sprite background(backgroundTex);
  sf::Texture foregroundTex;
  foregroundTex.loadFromFile("../graphic/hill_foreground.png");
  sf::Sprite foreground(foregroundTex);

  Moon moon("../graphic/moon.png", sf::Vector2i(800, 600));
  Starfield starfield(150, "../graphic/star.png", sf::Vector2i(800, 600));
  
  //Wind wind(0.3, sf::Vector2i(800, 600), "../graphic/wind.png", 2.0f, true);
  Breeze breeze(sf::Vector2i(800, 600), "../graphic/wind.png", 3, 2, 0.2, 0.4);
  
  //text stuff
  sf::Font mf;
  mf.loadFromFile("amiri-regular.ttf");
  sf::ComplexText text;
  sf::ComplexText text2;
  sf::String s;
  text.setString(s);
  text2.setString(s);
  text2.setStyle(sf::ComplexText::Stroked);
  text.setFont(mf);
  text2.setFont(mf);
  text.setCharacterSize (50);
  text2.setCharacterSize (50);
  text.setColor(sf::Color::Black);
  text2.setColor(sf::Color(100,100,100));
  text.setPosition(sf::Vector2f(50,50));
  text2.setPosition(sf::Vector2f(50,50));
  //text.setColor(sf::Color(255, 255, 255));

  // The main loop - ends as soon as the window is closed
  while (window.isOpen())
  {
    if (s == sf::String(L"عفيف")) {
      s.clear();
      text.setString(s);
      text2.setString(s);
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
            text.setString(s);
            text2.setString(s);
          } else {
            s += event.text.unicode;
            text.setString(s);
            text2.setString(s);
          }
        }
    }
    //wind.update();
    breeze.update();
    moon.updatePosition();
    starfield.update();
    // Clear the whole window before rendering a new frame
    window.clear(sf::Color(255,255,255));
    // Draw some graphical entities
    
    window.draw(background);
    
    window.draw(starfield);
    window.draw(moon);
    window.draw(foreground);
    window.draw(text2);
    window.draw(text);
    
    //window.draw(wind);
    
    window.draw(breeze);
    // End the current frame and display its contents on screen
    window.display();
  }
}
