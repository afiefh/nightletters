ostream& operator<<(ostream& o, const sf::Vector2f & v) {
  return o << "V2f[" << v.x << ", " << v.y << "]";
}

ostream& operator<<(ostream& o, const sf::Vector2i & v) {
  return o << "V2i[" << v.x << ", " << v.y << "]";
}

ostream& operator<<(ostream& o, const sf::Vertex & v) {
  return o << "Vert[" << v.position.x << ", " << v.position.y << "]";
}
