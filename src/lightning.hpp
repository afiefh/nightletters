#ifndef LIGHTNING_HPP
#define LIGHTNING_HPP
#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include "utilities.hpp"

const float PI = 3.14159265359f;
class Lightning : public sf::Drawable, public sf::Transformable {
public:
  Lightning(const sf::Vector2f& begin, const sf::Vector2f& end, sf::Vector2f perp, float widthBegin, float widthEnd, int maxAlpha) : m_vertices(sf::Quads), m_widthBegin(widthBegin), m_widthEnd(widthEnd), m_maxAlpha(maxAlpha) {
    m_texture.loadFromFile("../graphic/lightning2.png");
    generateLightning(begin, end, perp);
  }

  void generateLightning(const sf::Vector2f begin, const sf::Vector2f end, sf::Vector2f perp) {
    m_bolt.clear();
    m_vertices.clear();
    m_bolt.push_back(begin);
    m_bolt.push_back(end);
    std::vector<sf::Vector2f> tmpBolt;
    
    for (size_t subdivision=0; subdivision<7; subdivision++) {
      tmpBolt.clear();
      tmpBolt.push_back(m_bolt[0]);
      for (size_t i=1; i<m_bolt.size();i++) {
        //for any subsequent point
        float division = (rand()%20 + 40) / 100.0f;
        sf::Vector2f direction = m_bolt[i] - m_bolt[i-1];
        float length = getLength(direction);
        sf::Vector2f midpoint = m_bolt[i-1] + division * (m_bolt[i] - m_bolt[i-1]) + ((rand()%2)-1) * 0.15f*length*perp;
        if (getLength(m_bolt[i]-midpoint) < 2) continue;
        tmpBolt.push_back(midpoint);
        tmpBolt.push_back(m_bolt[i]);
      }
      std::swap(m_bolt, tmpBolt);
    }
    
    for (size_t i=1; i<m_bolt.size();i++) {
      const float width = m_widthBegin + (m_widthEnd - m_widthBegin) * (i-1) / m_bolt.size();
      
      sf::Vector2f dir = m_bolt[i] - m_bolt[i-1];
      dir = dir / getLength(dir) * (width/2); //we need with for the length of the cap
      sf::Vector2f perp = getPerpendicularDir(dir)*width; //twice as much as the length
      //pre cap
      m_vertices.append(sf::Vertex( m_bolt[i-1] + perp, sf::Vector2f(0,0) ));
      m_vertices.append(sf::Vertex( m_bolt[i-1] - perp, sf::Vector2f(0,128) ));
      m_vertices.append(sf::Vertex( m_bolt[i-1] - dir - perp, sf::Vector2f(64,128) ));
      m_vertices.append(sf::Vertex( m_bolt[i-1] - dir + perp, sf::Vector2f(64,0) ));
      
      // middle
      m_vertices.append(sf::Vertex( m_bolt[i-1] + perp, sf::Vector2f(0,0) ));
      m_vertices.append(sf::Vertex( m_bolt[i-1] - perp, sf::Vector2f(0,128) ));
      m_vertices.append(sf::Vertex( m_bolt[ i ] - perp, sf::Vector2f(1,128) ));
      m_vertices.append(sf::Vertex( m_bolt[ i ] + perp, sf::Vector2f(1,0) ));
      
      m_vertices.append(sf::Vertex( m_bolt[ i ] + perp, sf::Vector2f(0,0) ));
      m_vertices.append(sf::Vertex( m_bolt[ i ] - perp, sf::Vector2f(0,128) ));
      m_vertices.append(sf::Vertex( m_bolt[ i ] + dir - perp, sf::Vector2f(64,128) ));
      m_vertices.append(sf::Vertex( m_bolt[ i ] + dir + perp, sf::Vector2f(64,0) ));
      
    }
  }
  
  void draw(sf::RenderTarget& target, sf::RenderStates states) const
  {
    states.transform *= getTransform();
    states.texture    = &m_texture;
    target.draw(m_vertices, states);
  }

  void update(int iteration) {
    float alpha = m_maxAlpha - iteration;
    if(alpha < 0) {
      alpha = 0;
    }
    const float normalizedAlpha = (alpha / 255);
    const float realAlpha = normalizedAlpha*normalizedAlpha*normalizedAlpha*normalizedAlpha * 255;
    
    for(size_t i = 0; i < m_vertices.getVertexCount(); i++) {
      m_vertices[i].color.a = realAlpha;
    }
  }
  
  //returns coordinates and width at that point
  std::tuple<sf::Vector2f, float> getRandomPoint() const {
    size_t i = rand() % m_bolt.size();
    return std::make_tuple(m_bolt[i], getWidthAt(i));
  }
  int getMaxAlpha() {
    return m_maxAlpha;
  }
private:
  float getWidthAt(size_t i) const {
    return m_widthBegin + (m_widthEnd - m_widthBegin) * i / m_bolt.size();
  }
  sf::Vector2f getPerpendicularDir(sf::Vector2f dir) const { //TODO: put it in a common place so wind and lightning can use it
    float length = sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length == 0) throw std::runtime_error("perp of zero vector!");
    sf::Vector2f perpDir(-dir.y / length, dir.x / length);
    
    return perpDir;
  }
  float getLength(sf::Vector2f vec) const {
    return sqrt(vec.x * vec.x + vec.y * vec.y);
  }
  
  
  sf::Texture                m_texture;
  std::vector<sf::Vector2f>  m_bolt;
  sf::VertexArray            m_vertices;
  float                      m_widthBegin, m_widthEnd;
  int                        m_maxAlpha;
};

class LightningBolt : public sf::Drawable, public sf::Transformable {
public:
  LightningBolt(const sf::Vector2f& begin, const sf::Vector2f& end, size_t bolts, sf::Vector2i variance) : m_begin(begin), m_end(end), m_bolts(bolts), m_variance(variance), m_iteration(0) {
    generateLightnings();
  }
  
  void generateLightnings() {
    m_lightnings.clear();
    sf::Vector2f perp = getPerpendicularDir(m_begin - m_end);
    m_lightnings.push_back(Lightning(m_begin, m_end, perp, 40, 5, 255));
    for (size_t i = 0; i < m_bolts; i++) {
      sf::Vector2f midPoint;
      float width;
      size_t sourceLightning = rand() % m_lightnings.size();
      std::tie(midPoint,width) = m_lightnings[sourceLightning].getRandomPoint();
      int varY = m_variance.y == 0 ? 0 : rand() % m_variance.y;
      int realY = varY + m_end.y;
      int varX;
      if (realY - midPoint.y == 0) {
        varX = 0;
      } else {
        varX = m_variance.x == 0 ? 0 : ((rand() % 2) - 1) * (rand() % (abs(realY - midPoint.y)+1));
      }
         
      sf::Vector2f realEnd(m_end.x + varX, realY);
      int newAlpha = std::min(m_lightnings[sourceLightning].getMaxAlpha(), 200 + (rand()%55));
      m_lightnings.push_back(Lightning(midPoint, m_end + sf::Vector2f(varX, varY), perp, width, 5, newAlpha));
    }
  }
  
  void draw(sf::RenderTarget& target, sf::RenderStates states) const
  {
    states.transform *= getTransform();
    for (auto& lightning : m_lightnings)
      target.draw(lightning, states);
  }
  
  void update() {
    m_iteration++;
    if(m_iteration > 255) {
      generateLightnings();
      m_iteration = 0;
    }
    for (auto& lightning : m_lightnings)
      lightning.update(m_iteration);
    
  }
  
private:
  sf::Vector2f getPerpendicularDir(sf::Vector2f dir) const { //TODO: put it in a common place so wind and lightning can use it
    float length = sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length == 0) throw std::runtime_error("perp of zero vector!");
    sf::Vector2f perpDir(-dir.y / length, dir.x / length);
    
    return perpDir;
  }
  
  std::vector<Lightning> m_lightnings;
  sf::Vector2f           m_begin, m_end;
  size_t                 m_bolts;
  sf::Vector2i           m_variance;
  int                    m_iteration;
};

#endif // LIGHTNING_HPP
