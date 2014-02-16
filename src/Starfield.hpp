#ifndef __STARFIELD_HPP__
#define __STARFIELD_HPP__

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
      star.setCycle(scale * 0.05 / 100, (rand()%2000) / 2000.0f, (rand() % 314) / 100.0f, (10 + rand() % 40) / 100.0f);
      star.setScale(scale * 0.05 / 100, scale * 0.05 / 100);
      star.setRotation(rand() % 360);
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

#endif // __STARFIELD_HPP__