#include <SFML/Graphics.hpp>
#include "bezier.hpp"


class Wind : public sf::Drawable, public sf::Transformable {
public:
  Wind(float length, sf::Vector2i windowSize, const sf::Texture& texture, float thickness);
  void pickPoints();
  void update();
  void draw(sf::RenderTarget& target, sf::RenderStates states) const;
  void start(const sf::Vector2f& startPos, const sf::Vector2f& midPos, const sf::Vector2f& endPos, bool swirl);
  
  bool m_finished;
private:
  void generateGeometry();
  sf::Vector2f addSegment(sf::Vector2f p1, sf::Vector2f p2, float uv_start, float uv_end, const sf::Vector2f& previous_dir);
  sf::Vector2f getPerpendicularDir(sf::Vector2f dir);
  
  sf::Vector2i        m_windowSize;
  const sf::Texture & m_texture;
  BezierCurve         m_curve;
  sf::VertexArray     m_vertices;
  float               m_t;
  float               m_length;
  float               m_thickness;
};
