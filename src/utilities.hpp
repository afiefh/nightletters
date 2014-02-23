#ifndef __UTILITIES_HPP__
#define __UTILITIES_HPP__
#include <iostream>
std::ostream& operator<<(std::ostream& o, const sf::Vector2f & v);

std::ostream& operator<<(std::ostream& o, const sf::Vector2i & v);

std::ostream& operator<<(std::ostream& o, const sf::Vertex & v);

#endif // __UTILITIES_HPP__