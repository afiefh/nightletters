#ifndef __BEZIER_HPP__
#define __BEZIER_HPP__
using sf::Vector2f;

class BezierSegment {
public:
  BezierSegment() : m_length(0.0f) {}
  BezierSegment(float length) : m_length(length) {}
  virtual ~BezierSegment() {}
  virtual Vector2f getPoint(float t) const = 0; // 0 < t < 1
  float getLength() const { return m_length; }
protected:
  float distance(Vector2f v) { 
    return sqrt(v.x * v.x + v.y * v.y);
  }
  
  float m_length;
};

class BezierSegmentQuadratic : public BezierSegment {
public:
  BezierSegmentQuadratic() : m_p0(0, 0), m_p1(0, 0), m_p2(0, 0) {}
  BezierSegmentQuadratic(Vector2f p0, Vector2f p1, Vector2f p2) : 
    BezierSegment(distance(p1 - p0) + distance(p2 - p1)), 
    m_p0(p0), m_p1(p1), m_p2(p2) 
  {}
  
  virtual Vector2f getPoint(float t) const
  { return t*t*m_p2 + (1-t) * (2*t*m_p1 + (1-t)*m_p0); }
  
  void setPoints(Vector2f p0, Vector2f p1, Vector2f p2) {
    m_p0 = p0;
    m_p1 = p1;
    m_p2 = p2;
    m_length = distance(p1 - p0) + distance(p2 - p1);
  }
private:
  Vector2f m_p0, m_p1, m_p2;
};

class BezierSegmentCubic : public BezierSegment {
public:
  BezierSegmentCubic(Vector2f p0, Vector2f p1, Vector2f p2, Vector2f p3) : 
    BezierSegment(distance(p1 - p0) + distance(p2 - p1) + distance(p3 - p2)), 
    m_p0(p0), m_p1(p1), m_p2(p2), m_p3(p3) 
  {}
  
  virtual Vector2f getPoint(float t) const
  { return t*t*t*m_p3 + (1-t)*(3*t*t*m_p2 + (1-t)*(3*t*m_p1 + (1-t)*m_p0)); }
private:
  Vector2f m_p0, m_p1, m_p2, m_p3;
};


class BezierCurve {
public:
  void addSegment(BezierSegment * segment) {
    m_length += segment->getLength();
    m_segments.push_back(segment);
  }
  
  Vector2f getPoint(float t) const {
    if (m_segments.size() == 0) {
      throw std::runtime_error("Tried to get a point of a curve with zero segments");
    }
    
    if (t <=0 ) {
      float negativeLength = t * m_length;
      float newPercentage = negativeLength / m_segments[0]->getLength();
      return m_segments[0]->getPoint(newPercentage);
    } else if(t >= 1) {
      size_t lastItem = m_segments.size() - 1;
      float excessiveLength = t * m_length - m_length + m_segments[lastItem]->getLength();
      float newPercentage = excessiveLength / m_segments[lastItem]->getLength();
      return m_segments[lastItem]->getPoint(newPercentage);
    }
    
    const float desiredLength = t * m_length;
    float accumulatedLength = 0;
    for(BezierSegment * pSegment : m_segments) {
      float length = pSegment->getLength();
      if (accumulatedLength + length > desiredLength) {
        float relativeT = (desiredLength-accumulatedLength)/length;
        return pSegment->getPoint(relativeT);
      }
      accumulatedLength += length;
    }
    
    throw std::runtime_error("For some reason we reached an undefined state in the function BezierCurve::getPoint"); // BUG if you reach this!
  }
  
  void reset() {
    for(BezierSegment * pSegment : m_segments) {
      delete pSegment;
    }
    m_segments.clear();
    m_length = 0;
  }
private:
  float                       m_length;
  std::vector<BezierSegment*> m_segments;
};

#endif // __BEZIER_HPP__