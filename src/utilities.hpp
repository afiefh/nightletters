#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
std::ostream& operator<<(std::ostream& o, const sf::Vector2f & v);

std::ostream& operator<<(std::ostream& o, const sf::Vector2i & v);

std::ostream& operator<<(std::ostream& o, const sf::Vertex & v);

template<typename T>
std::ostream& operator<<(std::ostream& o, const std::vector<T> & v) {
  o << "std::Vector[";

  //first element gets no comma
  auto it = v.cbegin();
  o << *it;

  // all others do
  for(++it; it != v.cend(); ++it) {
    o << ", " << *it;
  }

  o << "]";
  return o;
}

#endif // __UTILITIES_HPP__
