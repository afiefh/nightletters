#include "wind.hpp"

Wind::Wind(float length, sf::Vector2i windowSize, const sf::Texture & texture, float thickness) 
    : m_finished(false), m_windowSize(windowSize), m_texture(texture), m_vertices(sf::Quads),
      m_t(0), m_length(length), m_thickness(thickness)
{ }

void Wind::generateGeometry() {
  m_vertices.clear();
  
  const int   segments = 20;
  
  const float segmentLength = m_length/segments;
  
  sf::Vector2f p0 = m_curve.getPoint(m_t - m_length);
  sf::Vector2f p1 = m_curve.getPoint(m_t - (segments - 1) * segmentLength);
  sf::Vector2f previousPerp = getPerpendicularDir(p1 - p0) * m_thickness;
  addSegment(p0, p1, 100, 0, previousPerp );
  
  for(int i=1; i < segments-1; i++) {
    p0 = m_curve.getPoint(m_t - (segments - i) * segmentLength);
    p1 = m_curve.getPoint(m_t - (segments - i - 1) * segmentLength);
    previousPerp = addSegment(p0, p1, 0, 1, previousPerp);
  }
  
  // last segment has special UVs:
  p0 = m_curve.getPoint(m_t - segmentLength);
  p1 = m_curve.getPoint(m_t);
  addSegment(p0, p1, 0, 100, previousPerp);
}
  
void Wind::update() {
  if (m_finished) return;
  
  if (m_t > 1.0f + m_length) {
    m_t = 0;
    m_finished = true;
    return;
  }
  
  m_t += 0.01;
  generateGeometry();
}
  
void Wind::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  states.transform *= getTransform();
  states.texture = &m_texture;
  target.draw(m_vertices, states);
}

sf::Vector2f Wind::addSegment(sf::Vector2f p1, sf::Vector2f p2, float uv_start, float uv_end, const sf::Vector2f& previousPerp) {
  m_vertices.append(sf::Vertex( p1 + previousPerp, sf::Vector2f(uv_start, 0) ));
  m_vertices.append(sf::Vertex( p1 - previousPerp, sf::Vector2f(uv_start, 100) ));
  
  sf::Vector2f currentPerp = getPerpendicularDir(p2 - p1) * m_thickness;
  m_vertices.append(sf::Vertex( p2 - currentPerp, sf::Vector2f(uv_end, 100) ));
  m_vertices.append(sf::Vertex( p2 + currentPerp, sf::Vector2f(uv_end, 0) ));
  return currentPerp;
}

sf::Vector2f Wind::getPerpendicularDir(sf::Vector2f dir) {
  float length = sqrt(dir.x * dir.x + dir.y * dir.y);
  sf::Vector2f perpDir(-dir.y / length, dir.x / length);
  
  return perpDir;
}

void Wind::start(const sf::Vector2f& startPos, const sf::Vector2f& midPos, const sf::Vector2f& endPos, bool swirl) {
  m_finished = false;
  m_curve.reset();
  m_t = -(rand() % 100 / 1000.0f); //because eran insisted on non-random numbers!
  
  sf::Vector2f p0 = startPos + sf::Vector2f(0, rand()%200-100);
  sf::Vector2f p2 = endPos + sf::Vector2f(0, rand()%200-100);
    
  if (swirl) {
    //basic points that the path will move along, in the middle we'll have a little swirl
    float radius = 30 + rand()%20;
    sf::Vector2f swirlBottom = midPos  + sf::Vector2f(rand()%200-100, rand()%200-100);
    swirlBottom.y = std::max(radius*2, swirlBottom.y);
    // now since we know that the radius of the swirl is 20 we can draw the swirl out:
    sf::Vector2f swirlCenter = swirlBottom - sf::Vector2f(0, radius); //temporary to make math more intuitive
    
    sf::Vector2f swirlTop   = swirlCenter - sf::Vector2f(0, radius);
    sf::Vector2f swirlRight = swirlCenter + sf::Vector2f(2*radius, 0);
    sf::Vector2f swirlLeft  = swirlCenter - sf::Vector2f(2*radius, 0);
    
    sf::Vector2f swirlTopLeft  = swirlTop - sf::Vector2f(radius / 2, 0);
    sf::Vector2f swirlTopRight = swirlTop + sf::Vector2f(radius / 2, 0);
    
    //the point should be half the distance between p0 and swirlbottom on the X axis, but there should be a straight line between mid1,swirlBottom,swirlRight
    sf::Vector2f diff1 = swirlRight - swirlBottom;
    float diffx1 = (swirlBottom.x - p0.x)/2;
    sf::Vector2f mid1 = swirlBottom - diff1 * (diffx1/diff1.x/2);
    
    //same for the other direction
    sf::Vector2f diff2 = swirlLeft - swirlBottom;
    float diffx2 = (swirlBottom.x - p2.x)/2;
    sf::Vector2f mid2 = swirlBottom - diff2 * (diffx2/diff2.x/2);
    
    m_curve.addSegment(new BezierSegmentQuadratic(p0, mid1, swirlBottom));                        // start to loop
    m_curve.addSegment(new BezierSegmentCubic(swirlBottom, swirlRight, swirlTopRight, swirlTop)); // loop right
    m_curve.addSegment(new BezierSegmentCubic(swirlTop, swirlTopLeft, swirlLeft, swirlBottom));   // loop left
    m_curve.addSegment(new BezierSegmentQuadratic(swirlBottom, mid2, p2));                        // loop to end
  } else {
    sf::Vector2f p11 = p0+(midPos - p0)/2.0f  + sf::Vector2f(0, rand()%200-100);
    sf::Vector2f p12 = midPos+(p2 - midPos)/2.0f  + sf::Vector2f(0, rand()%200-100);
    m_curve.addSegment(new BezierSegmentCubic(p0, p11, p12, p2));
  }
}