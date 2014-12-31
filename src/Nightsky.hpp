#ifndef __NIGHTSKY__HPP__
#define __NIGHTSKY__HPP__

#include "bezier.hpp"
#include "Breeze.hpp"
#include "Moon.hpp"
#include "Starfield.hpp"

class Nightsky : public sf::Drawable {
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
  
  virtual void  draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(m_background, states);
    target.draw(m_starfield, states);
    target.draw(m_moon, states);
    target.draw(m_foreground, states);
    target.draw(m_breeze, states);
  }
  
  const Breeze& getBreeze() const { return m_breeze; }
  Starfield& getStarfield() { return m_starfield; }
  
private:
  sf::Texture m_backgroundTex;
  sf::Texture m_foregroundTex;
  
  sf::Sprite m_background;
  sf::Sprite m_foreground;

  Moon m_moon;
  Starfield m_starfield;
  Breeze m_breeze;
};

#endif // __NIGHTSKY__HPP__