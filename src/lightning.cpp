#include "lightning.hpp"

LightningBolt::LightningBolt(size_t bolts, const sf::Vector2i& boltSize)
    : m_bolts(bolts), m_boltSize(boltSize), m_duration(sf::seconds(1))
{
  generateLightnings();
}

void LightningBolt::generateLightnings() 
{
  const sf::Vector2f begin = sf::Vector2f(m_boltSize.x/2, 0);
  const sf::Vector2f end   = sf::Vector2f(m_boltSize.x/2, m_boltSize.y-1);
  std::cout << "Bolt from: " << begin << " to " << end << std::endl;
  m_lightnings.clear();
  sf::Vector2f perp = sf::Vector2f(1, 0);
  m_lightnings.push_back(Lightning(begin, end, perp, 40, 10, 255));
  for (size_t i = 0; i < m_bolts; i++) {
    sf::Vector2f midPoint;
    float width;
    size_t sourceLightning = rand() % m_lightnings.size();
    std::tie(midPoint, width) = m_lightnings[sourceLightning].getRandomPoint();
    const int endY = m_boltSize.y - (rand() % (m_boltSize.y/10));
    const int distanceY = abs(endY - midPoint.y);
    int endX = midPoint.x + (rand() % (2 * distanceY)) - distanceY;
    if(endX < 0) endX = 0;
    if(endX > m_boltSize.x) endX = m_boltSize.x;
    sf::Vector2f  endPoint = sf::Vector2f(endX, endY);
    
    
    int newAlpha = std::min(m_lightnings[sourceLightning].getMaxAlpha(), 200 + (rand()%55));
    m_lightnings.push_back(Lightning(midPoint, endPoint, perp, width, 5, newAlpha));
  }
}

void LightningBolt::draw(sf::RenderTarget& target, sf::RenderStates states) const
{

  if (isFinished()) //nothing to draw
    return;
    
  states.transform *= getTransform();
  for (auto& lightning : m_lightnings)
    target.draw(lightning, states);
}

void LightningBolt::update(const sf::Time &dt) {
  if(isFinished()) {
    return;
  }
  
  m_elapsedTime += dt;
  const float ratio = m_elapsedTime.asSeconds()/m_duration.asSeconds();
  for (auto& lightning : m_lightnings) {
    lightning.update(1 - ratio);
  }
}

void LightningBolt::start() {
  m_elapsedTime = sf::Time();
  generateLightnings();
}

sf::Vector2f LightningBolt::getPerpendicularDir(sf::Vector2f dir) const { //TODO: put it in a common place so wind and lightning can use it
  float length = sqrt(dir.x * dir.x + dir.y * dir.y);
  if (length == 0) throw std::runtime_error("perp of zero vector!");
  sf::Vector2f perpDir(-dir.y / length, dir.x / length);
  
  return perpDir;
}


void FlashLightning::update(const sf::Time &dt) {
  m_elapsedTime += dt;
  m_lightningBolt.update(dt);
  
  m_framebuffer.clear(sf::Color(0,0,0,0));
  glBlendEquation(GL_MAX);
  m_framebuffer.draw(m_lightningBolt);
  m_framebuffer.display();
  glBlendEquation(GL_FUNC_ADD);
  
  const int alpha = (2*m_elapsedTime.asSeconds())*255/m_duration.asSeconds();
  m_rect.setFillColor(sf::Color(255,255,255,std::max(255-alpha, 0)));
}