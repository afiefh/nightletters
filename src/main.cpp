
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

#include "ComplexText.hpp"
#include "bezier.hpp"

#include <cwchar>
#include <iostream>
#include <cassert>
#include <stdio.h>
#include <cmath>

using namespace std;

ostream& operator<<(ostream& o, const sf::Vector2f & v) {
  return o << "V2f[" << v.x << ", " << v.y << "]" << endl;
}

ostream& operator<<(ostream& o, const sf::Vector2i & v) {
  return o << "V2i[" << v.x << ", " << v.y << "]" << endl;
}

ostream& operator<<(ostream& o, const sf::Vertex & v) {
  return o << "Vert[" << v.position.x << ", " << v.position.y << "]" << endl;
}

sf::Vector2f bezier3(const sf::Vector2f & p0, const sf::Vector2f & p1, const sf::Vector2f & p2, float t) {
  return (1-t) * ((1-t)*p0 + 2*t*p1) + t*t*p2;
}

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


class Star : public sf::Sprite {
public:
  void setBasePosition(float x, float y) {
    m_x = x;
    m_y = y;
  }
  void setCycle(float baseScale, float omega, float theta, float variance) {
    m_baseScale = baseScale;
    m_omega     = omega;
    m_theta     = theta;
    m_variance  = variance;
  }
  
  void update(float t) {
    float newScale = m_baseScale + m_variance * m_baseScale * sin(m_omega*t+m_theta);
    setScale(newScale, newScale);
    //
  }
  
  float m_baseScale;
  float m_omega, m_theta, m_variance;
  float m_x, m_y;
};

sf::Vector2f veci2f(const sf::Vector2i & v) {
  return sf::Vector2f(v.x,v.y);
}



class Wind : public sf::Drawable, public sf::Transformable {
public:
  Wind(float length, sf::Vector2i windowSize, const char * texture) : m_windowSize(windowSize), m_vertices(sf::Quads), m_t(0), m_length(length) {
    m_texture.loadFromFile(texture);
  }
  
  void pickPoints() {
    m_curve.reset();
    
    sf::Vector2f p0 = sf::Vector2f(0, rand() % m_windowSize.y/2);
    sf::Vector2f p2 = sf::Vector2f(m_windowSize.x, rand() % m_windowSize.y/2);
    sf::Vector2f p1 = (p0 + p2)/2.0f  + sf::Vector2f(0, rand()%400-200);
    m_curve.addSegment(new BezierSegmentQuadratic(p0, p1, p2));
    
    generateGeometry();
  }

  void generateGeometry() {
    m_vertices.clear();
    
    const int   segments = 20;
    
    const float segmentLength = m_length/segments;
    
    sf::Vector2f p0 = m_curve.getPoint(m_t - m_length);
    sf::Vector2f p1 = m_curve.getPoint(m_t - (segments - 1) * segmentLength);
    addSegment(p0, p1, 100, 0);
    
    for(int i=1; i < segments-1; i++) {
      p0 = m_curve.getPoint(m_t - (segments - i) * segmentLength);
      p1 = m_curve.getPoint(m_t - (segments - i - 1) * segmentLength);
      addSegment(p0, p1, 0, 1);
    }
    
    // last segment has special UVs:
    p0 = m_curve.getPoint(m_t - segmentLength);
    p1 = m_curve.getPoint(m_t);
    addSegment(p0, p1, 0, 100);
  }
  
  void update() {
    if (m_t > 1.0f + m_length) {
      m_t = 0;
      pickPoints();
    } else {
      m_t += 0.01;
      generateGeometry();
    }

  }
  
  void draw(sf::RenderTarget& target, sf::RenderStates states) const
  {
    states.transform *= getTransform();
    states.texture = &m_texture;
    target.draw(m_vertices, states);
  }
  
private:

  void addSegment(sf::Vector2f p1, sf::Vector2f p2, float uv_start, float uv_end) {
    const sf::Vector2f vecy(0, 5);
    
    m_vertices.append(sf::Vertex( p1 + vecy, sf::Vector2f(uv_start, 0) ));
    m_vertices.append(sf::Vertex( p1 - vecy, sf::Vector2f(uv_start, 100) ));
    
    m_vertices.append(sf::Vertex( p2 - vecy, sf::Vector2f(uv_end, 100) ));
    m_vertices.append(sf::Vertex( p2 + vecy, sf::Vector2f(uv_end, 0) ));
  }
  
  sf::Vector2i      m_windowSize;
  BezierCurve       m_curve;
  sf::VertexArray   m_vertices;
  sf::Texture       m_texture;
  float             m_t;
  float             m_length;
};

class Starfield : public sf::Drawable {
public:
  Starfield(int n, const char * starImage, sf::Vector2i windowSize) {
    t=0;
    stars.resize(n);
    starTex.loadFromFile(starImage);
    
    for(auto& star : stars) {
      star.setTexture(starTex, true);
      star.setOrigin(star.getTextureRect().width/2, star.getTextureRect().height/2);
      
      int scale = rand() % 100;
      star.setCycle(scale * 0.05/100, (rand()%2000) / 2000.0f, (rand()%314)/100.0f, (10+rand()%40) / 100.0f);
      star.setScale(scale * 0.05/100, scale*0.05/100);
      star.setRotation(rand()%360);
      star.setPosition(rand() % windowSize.x, rand() % windowSize.y);
    }
  }
  
  virtual void  draw (sf::RenderTarget &target, sf::RenderStates states) const {
    for(auto& star : stars) {
      target.draw(star, states);
    }
  }
  
  void update() {
    for(auto& star : stars) {
      star.update(t);
      //star.setRotation(t*10);
    }
    t+=0.05;
  }
private:
  sf::Vector2i windowSize;
  sf::Texture starTex;
  std::vector<Star> stars;
  float t;
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
  
  Wind wind(0.3, sf::Vector2i(800, 600), "../graphic/wind.png");
  wind.pickPoints();
  
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
    wind.update();
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
    
    window.draw(wind);
    // End the current frame and display its contents on screen
    window.display();
  }
}
