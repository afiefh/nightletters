#ifndef __MOON_HPP__
#define __MOON_HPP__

#include <SFML/Graphics.hpp>
#include "bezier.hpp"

class Moon : public sf::Sprite {
public:
  Moon(const char * image, sf::Vector2i ws) : t(0.0) {
    moonTex.loadFromFile(image);
    setTexture(moonTex, true);
    
    const sf::IntRect & rect = getTextureRect();
    const sf::Vector2f s = sf::Vector2f(rect.width, rect.height);
    sf::Vector2f p0 = sf::Vector2f(ws.x + s.x/2, ws.y/3);
    sf::Vector2f p1 = sf::Vector2f(0, 0);
    sf::Vector2f p2 = sf::Vector2f(ws.x/3, ws.y + s.y/2);
    curve.setPoints(p0, p1, p2);
  }
  
  void updatePosition() {
    //float r = 300;
    sf::Vector2f position = curve.getPoint(t);
    sf::Vector2f finalPos = position - sf::Vector2f(getTextureRect().width/2,getTextureRect().height/2);
    setPosition( finalPos.x, finalPos.y);
    //move(0.01,0);
    t += 0.0001;
    if (t > 1) t=0;
  }
  
private:
  sf::Texture moonTex;
  BezierSegmentQuadratic curve;
  float t;
};

#endif // __MOON_HPP__