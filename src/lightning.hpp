#ifndef LIGHTNING_HPP
#define LIGHTNING_HPP
#include <SFML/Graphics.hpp>
#include <vector>
#include <ctime>
#include <iostream>
#include "utilities.hpp"

const float PI = 3.14159265359f;
class Lightning : public sf::Drawable, public sf::Transformable {
public:
  Lightning(const sf::Vector2f& begin, const sf::Vector2f& end, sf::Vector2f perp, float widthBegin, float widthEnd) : m_vertices(sf::Quads), m_alpha(255) , m_widthBegin(widthBegin), m_widthEnd(widthEnd) {
    m_texture.loadFromFile("../graphic/lightning2.png");
    generateLightning(begin, end, perp);
  }

  void generateLightning(const sf::Vector2f begin, const sf::Vector2f end, sf::Vector2f perp) {
    m_bolt.clear();
    m_vertices.clear();
    m_bolt.push_back(begin);
    m_bolt.push_back(end);
    std::vector<sf::Vector2f> tmpBolt;
    
    for (size_t subdivision=0;subdivision<8;subdivision++) {
      tmpBolt.clear();
      tmpBolt.push_back(m_bolt[0]);
      for (size_t i=1; i<m_bolt.size();i++) {
        //for any subsequent point
        float division = (rand()%20 + 40) / 100.0f;
        sf::Vector2f direction = m_bolt[i] - m_bolt[i-1];
        float length = getLength(direction);
        sf::Vector2f midpoint = m_bolt[i-1] + ((division < 1-division)? division : 1-division) *(m_bolt[i] - m_bolt[i-1]) + ((rand()%2)-1) * 0.35f*length*perp;
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

  void update() {
    m_alpha -= 1;
    if(m_alpha < 0) {
      m_alpha = 255;
    }
    const float normalizedAlpha = (m_alpha/255);
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
private:
  float getWidthAt(size_t i) const {
    return m_widthBegin + (m_widthEnd - m_widthBegin) * (i-1) / m_bolt.size();
  }
  sf::Vector2f getPerpendicularDir(sf::Vector2f dir) const { //TODO: put it in a common place so wind and lightning can use it
    float length = sqrt(dir.x * dir.x + dir.y * dir.y);
    sf::Vector2f perpDir(-dir.y / length, dir.x / length);
    
    return perpDir;
  }
  float getLength(sf::Vector2f vec) const {
    return sqrt(vec.x * vec.x + vec.y * vec.y);
  }
  
  
  sf::Texture           m_texture;
  std::vector<sf::Vector2f>  m_bolt;
  sf::VertexArray       m_vertices;
  float                 m_alpha;
  float                 m_widthBegin, m_widthEnd;
};

class LightningBolt : public sf::Drawable, public sf::Transformable {
public:
  LightningBolt(const sf::Vector2f& begin, const sf::Vector2f& end, size_t bolts, sf::Vector2i variance) {
    sf::Vector2f perp = getPerpendicularDir(begin - end);
    std::cout << "Perp:" << perp << std::endl;
    m_lightnings.push_back(Lightning(begin, end, perp, 40, 5));
    for (size_t i = 0; i < bolts; i++) {
      sf::Vector2f midPoint;
      float width;
      std::tie(midPoint,width) = m_lightnings[rand() % (i+1)].getRandomPoint();
      int varX = variance.x == 0 ? 0 : rand() % variance.x;
      int varY = variance.y == 0 ? 0 : rand() % variance.y;
      m_lightnings.push_back(Lightning(midPoint, end + sf::Vector2f(varX, varY), perp, width, 5));
    }
  }
  
  
  void draw(sf::RenderTarget& target, sf::RenderStates states) const
  {
    states.transform *= getTransform();
    for (auto& lightning : m_lightnings)
      target.draw(lightning, states);
  }
  
  void update() {
    for (auto& lightning : m_lightnings)
      lightning.update();
  }
private:
  sf::Vector2f getPerpendicularDir(sf::Vector2f dir) const { //TODO: put it in a common place so wind and lightning can use it
    float length = sqrt(dir.x * dir.x + dir.y * dir.y);
    sf::Vector2f perpDir(-dir.y / length, dir.x / length);
    
    return perpDir;
  }
  
  std::vector<Lightning> m_lightnings;
};

#endif // LIGHTNING_HPP
