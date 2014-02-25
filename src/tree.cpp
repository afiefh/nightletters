#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <vector>
#include <iostream>
#include <cmath>

#include "utilities.hpp"

struct PointMass {
  sf::Vector2f position;
  sf::Vector2f acceleration;
  sf::Vector2f velocity;
  bool pinned;
};

class SoftBody : public sf::Drawable, public sf::Transformable {
public:
  SoftBody() : m_drag(0.9), m_vertices(sf::Points) {
    sf::Vector2f origin(0.0f,0.0f);
    for (size_t h=0; h < m_tesselationY; h++) {
      for (size_t h=0; h < m_tesselationX; h++) {
        m_masses.push_back( PointMass{origin, sf::Vector2f(0,0), sf::Vector2f(0,0), false} );
        origin.x += width/m_tesselationX;
      }
      origin.y += height/m_tesselationY;
      origin.x  = 0;
    }
    update();
  }
  
  inline sf::Vector2f CalcAcceleration(PointMass& self, PointMass& other, float restDistance) {
    sf::Vector2f diffPos  = self.position - other.position;
    float diffPosLen  = getLength(diffPos);
    return -(diffPosLen  - restDistance) * diffPos/diffPosLen;
  }
  inline float getLength(sf::Vector2f vec) { return sqrt(vec.x*vec.x  + vec.y*vec.y); }
  inline size_t getIndex(size_t x, size_t y) { return x + y * m_tesselationX; }
  void update() {
    //std::cout << "Process:" << std::endl;
    const float restDistanceX = width  / (float)m_tesselationX;
    const float restDistanceY = height / (float)m_tesselationY;
    
    for (size_t h = 1; h < m_tesselationY-1; h++) {
      for (size_t w = 1; w < m_tesselationX-1; w++) {
        size_t self  = getIndex(w    , h    );
        size_t left  = getIndex(w - 1, h    );
        size_t right = getIndex(w + 1, h    );
        size_t up    = getIndex(w    , h - 1);
        size_t down  = getIndex(w    , h + 1);
        
        sf::Vector2f diffLeft  = m_masses[self].position - m_masses[left].position;
        sf::Vector2f diffRight = m_masses[self].position - m_masses[right].position;
        //std::cout << "DiffLeft = " << diffLeft << " = " << m_masses[self].position << " - " << m_masses[left].position << std::endl;
        //std::cout << "DiffRight = " << diffLeft << " = " << m_masses[self].position << " - " << m_masses[right].position << std::endl;
        
        sf::Vector2f diffUp    = m_masses[self].position - m_masses[up].position;
        sf::Vector2f diffDown  = m_masses[self].position - m_masses[down].position;
        
        float lenLeft  = getLength(diffLeft);
        float lenRight = getLength(diffRight);
        
        float lenUp    = getLength(diffUp);
        float lenDown  = getLength(diffDown);
        
        sf::Vector2f acceleration(0,0);
        acceleration -= (lenLeft  - restDistanceX) * diffLeft/lenLeft;
        acceleration -= (lenRight - restDistanceX) * diffRight/lenRight;
        
        acceleration -= (lenUp   - restDistanceY) * diffUp/lenUp;
        acceleration -= (lenDown - restDistanceY) * diffDown/lenDown;
        
        m_masses[self].acceleration = acceleration/2.0f;
        //std::cout << "Acceleration: " << w << ", " << h<< ": " << acceleration << std::endl;
        //std::cout << "left:" << lenLeft << " right" << lenRight << " up:" << lenUp << " down:" << lenDown << std::endl;
      }
    }
    
    //update the position
    for(auto& pointMass : m_masses) {
      pointMass.velocity = pointMass.acceleration * 0.1f + pointMass.velocity * m_drag;
      pointMass.position += pointMass.velocity * 0.1f;
    }
    
    m_vertices.clear();
    for(auto& pointMass : m_masses) {
      m_vertices.append(sf::Vertex(pointMass.position, sf::Color::White));
    }
  }
  
  void draw(sf::RenderTarget& target, sf::RenderStates states) const
  {
    states.transform *= getTransform();
    target.draw(m_vertices, states);
  }
  
  void pushRight(float velocity, float acceleration) {
    std::cout << "Set velocity" << velocity << "acceleration=" << acceleration << std::endl;
    m_masses[getIndex(1,1)].acceleration.y += acceleration;
    m_masses[getIndex(1,1)].velocity.y     += velocity;
  }
private:
  //all the masses are in here
  std::vector<PointMass> m_masses;
  static const size_t m_tesselationX = 10;
  static const size_t m_tesselationY = 10;
  static const size_t width = 600;
  static const size_t height = 600;
  const float m_drag;
  
  sf::VertexArray       m_vertices;
};

int main()
{
  bool update = false;
  srand(time(NULL));
  sf::RenderWindow window(sf::VideoMode(800, 600), "Nightletters");
  window.setFramerateLimit(60);
  
  SoftBody softBody;
  softBody.move(20, 20);
  softBody.pushRight(1000000, 0);
  while (window.isOpen())
  {
    window.clear(sf::Color::Black);
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
      softBody.update();
    }
    window.draw(softBody);
    
    // End the current frame and display its contents on screen
    window.display();
  }
}
