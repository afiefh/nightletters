#ifndef __STORKEDTEXT_HPP__
#define __STORKEDTEXT_HPP__

#include "Font.hpp"
#include "ComplexText.hpp"

class StrokedText : public sf::Drawable {
public:
  StrokedText() {
    text2.setStyle(sf::ComplexText::Stroked);
  }
  void setString(const sf::String& s) {
    text.setString(s);
    text2.setString(s);
  }
  
  void setFont(sf::ComplexFont& font) {
    text.setFont(font);
    text2.setFont(font);
  }
  
  void setCharacterSize(int size) {
    text.setCharacterSize(size);
    text2.setCharacterSize(size);
  }
  
  void setColor(const sf::Color& foreground, const sf::Color& background) {
    text.setColor(foreground);
    text2.setColor(background);
  }
  
  void setPosition(const sf::Vector2f position) {
    text.setPosition(position);
    text2.setPosition(position);
  }
  
  virtual void  draw(sf::RenderTarget &target, sf::RenderStates states) const {
    target.draw(text2, states);
    target.draw(text, states);
  }
  
private:
  sf::ComplexText text;
  sf::ComplexText text2;
};

#endif // __STORKEDTEXT_HPP__