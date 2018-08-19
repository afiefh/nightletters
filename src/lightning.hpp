#ifndef LIGHTNING_HPP
#define LIGHTNING_HPP
#include <SFML/Graphics.hpp>
#include <GL/glew.h>
#include <vector>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include "utilities.hpp"
#include "Action.hpp"

const float PI = 3.14159265359f;
const size_t lightningSubdivision = 7;
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

    for (size_t subdivision=0; subdivision<lightningSubdivision; subdivision++) {
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

  void update(float modifier) {
    for(size_t i = 0; i < m_vertices.getVertexCount(); i++) {
      m_vertices[i].color.a = m_maxAlpha * modifier;
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

class LightningBolt : public sf::Drawable, public sf::Transformable, public Action {
public:
  LightningBolt(size_t bolts, const sf::Vector2i& boltSize);
  virtual void onStart() { start(); }
  virtual void onEnd() {}
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
  virtual void update(const sf::Time &dt);
  virtual void start();
  virtual bool isFinished() const { return (m_elapsedTime >= m_duration); }
  virtual bool isBlocking() const { return true; }

private:
  void generateLightnings();
  sf::Vector2f getPerpendicularDir(sf::Vector2f dir) const; //TODO: put it in a common place so wind and lightning can use it

  std::vector<Lightning> m_lightnings;
  size_t                 m_bolts;
  sf::Vector2i           m_boltSize;
  int                    m_iteration;
  sf::Time               m_elapsedTime;
  sf::Time               m_duration;
};


class FlashLightning : public sf::Drawable, public sf::Transformable, public Action {
public:
  FlashLightning(size_t bolts,
                 const sf::Vector2i& boltSize,
                 const sf::Time& duration,
                 const sf::Vector2i& windowSize,
                 const sf::Vector2f& position)
      : m_lightningBolt(bolts, boltSize),
        m_rect(sf::Vector2f(windowSize.x, windowSize.y)),
        m_elapsedTime(duration),
        m_duration(duration)
  {
    m_framebuffer.create(boltSize.x, boltSize.y);
    m_lightningSprite.setTexture(m_framebuffer.getTexture(), true);
    //m_lightningSprite.setPosition(position);
  }

  virtual void onStart() {
    m_lightningBolt.start();
    m_elapsedTime = sf::Time();
  }
  virtual void onEnd() {}
  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (isFinished()) //nothing to draw
      return;

    states.transform *= getTransform();
    //target.draw(m_lightningBolt, states);
    target.draw(m_lightningSprite, states);
    target.draw(m_rect, states);
  }

  virtual void update(const sf::Time &dt);
  virtual bool isFinished() const { return (m_elapsedTime >= m_duration); }
  virtual bool isBlocking() const { return true; }

private:
  LightningBolt        m_lightningBolt;
  sf::RectangleShape   m_rect;
  sf::Time             m_elapsedTime;
  sf::Time             m_duration;

  sf::RenderTexture    m_framebuffer;
  sf::Sprite           m_lightningSprite;
};

#endif // LIGHTNING_HPP
