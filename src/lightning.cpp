#include "lightning.hpp"

LightningBolt::LightningBolt(const sf::Vector2f& begin, const sf::Vector2f& end, size_t bolts, sf::Vector2i variance) 
    : m_begin(begin), m_end(end), m_bolts(bolts), m_variance(variance), m_elapsedTime(1) 
{
  generateLightnings();
}

void LightningBolt::generateLightnings() {
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
  
  m_elapsedTime += dt.asSeconds();
  for (auto& lightning : m_lightnings) {
    lightning.update(1 - m_elapsedTime);
  }
}

void LightningBolt::start() {
  m_elapsedTime = 0;
  generateLightnings();
}

sf::Vector2f LightningBolt::getPerpendicularDir(sf::Vector2f dir) const { //TODO: put it in a common place so wind and lightning can use it
  float length = sqrt(dir.x * dir.x + dir.y * dir.y);
  if (length == 0) throw std::runtime_error("perp of zero vector!");
  sf::Vector2f perpDir(-dir.y / length, dir.x / length);
  
  return perpDir;
}
