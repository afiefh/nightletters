#ifndef __BREEZE_HPP__
#define __BREEZE_HPP__

#include "wind.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>

class Breeze : public sf::Drawable {
public:
  Breeze(sf::Vector2i windowSize, const char* texture, size_t numberOfWinds, float thickness, float lengthMin, float lengthMax) : m_windowSize(windowSize) {
    m_texture.loadFromFile(texture);
    for(size_t i=0; i<numberOfWinds; i++) {
      m_winds.push_back(Wind(rand()%100 / 100.f * (lengthMax-lengthMin) + lengthMin, windowSize, m_texture, thickness));
    }
    startWinds();
  }

  void draw(sf::RenderTarget& target, sf::RenderStates states) const
  {
    for(auto& wind : m_winds) {
      target.draw(wind, states);
    }
  }

  void update() {
    bool allFinished = true;
    for(auto& wind : m_winds) {
      wind.update();
      allFinished = allFinished && wind.m_finished;
    }
    
    if (allFinished) {
      startWinds();
    }
  }
  
  std::pair<float, float> getStartAndEnd() const {
    if (m_winds.size() < 1)
      return std::pair<float, float>(0.0f, 0.0f);
    
    std::pair<float, float> result = m_winds[0].getStartAndEnd();
    for (auto it = ++m_winds.begin(); it != m_winds.end(); ++it) {
      std::pair<float, float> other = it->getStartAndEnd();
      result.first = std::min(result.first, other.first);
      result.second = std::max(result.second, other.second);
    }
    return result;
  }
  
private:
  void startWinds() 
  {
    sf::Vector2f startPos = sf::Vector2f(0, rand() % m_windowSize.y/2);
    sf::Vector2f endPos   = sf::Vector2f(m_windowSize.x, rand() % m_windowSize.y/2);
    sf::Vector2f midPos   = (startPos + endPos)/2.0f  + sf::Vector2f(0, rand()%400-200);
    for(auto& wind : m_winds) {
        wind.start(startPos, midPos, endPos, rand()%3 == 0);
      }
  }
  
  
private:
  std::vector<Wind> m_winds;
  sf::Texture       m_texture;
  sf::Vector2i      m_windowSize;
};

#endif // __BREEZE_HPP__