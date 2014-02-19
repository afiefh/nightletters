
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Audio.hpp>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

#include "Nightsky.hpp"
#include "StrokedText.hpp"
#include <cwchar>
#include <iostream>
#include <cassert>
#include <stdio.h>
#include <cmath>
#include <list>

#include <jsoncpp/json/json.h>
#include <fstream>
#include <algorithm>
using namespace std;

ostream& operator<<(ostream& o, const sf::Vector2f & v) {
  return o << "V2f[" << v.x << ", " << v.y << "]";
}

ostream& operator<<(ostream& o, const sf::Vector2i & v) {
  return o << "V2i[" << v.x << ", " << v.y << "]";
}

ostream& operator<<(ostream& o, const sf::Vertex & v) {
  return o << "Vert[" << v.position.x << ", " << v.position.y << "]";
}

sf::Vector2f bezier3(const sf::Vector2f & p0, const sf::Vector2f & p1, const sf::Vector2f & p2, float t) {
  return (1-t) * ((1-t)*p0 + 2*t*p1) + t*t*p2;
}

struct LightningNode {
  LightningNode(float length, float angle) : m_length(length), m_angle(angle), m_right(NULL), m_left(NULL) {}
  float m_length;
  float m_angle;
  
  LightningNode * m_right;
  LightningNode * m_left;
};
const float PI = 3.14159265359f;
class Lightning : public sf::Drawable, public sf::Transformable{
public:
  Lightning() : m_vertices(sf::Quads){
    std::cout << "Lightning constructed" << std::endl;
    m_texture.loadFromFile("../graphic/lightning.png");
    generateLightning();
  }

  void generateLightning() {
    LightningNode node1(100, 0);
    LightningNode node2(100, 45);
    LightningNode node3(100, -45);
    node1.m_right = &node2;
    node1.m_left  = &node3;
    std::pair<sf::Vector2f, sf::Vector2f> currentEnd(sf::Vector2f(90,100), sf::Vector2f(110,100));
    drawLightningSegment(90, 20, node1, currentEnd);
  }
  
  void draw(sf::RenderTarget& target, sf::RenderStates states) const
  {
    states.transform *= getTransform();
    states.texture = &m_texture;
    target.draw(m_vertices, states);
  }

private:
  sf::Vector2f getPerpendicularDir(sf::Vector2f dir) { //TODO: put it in a common place so wind and lightning can use it
    float length = sqrt(dir.x * dir.x + dir.y * dir.y);
    sf::Vector2f perpDir(-dir.y / length, dir.x / length);
    
    return perpDir;
  }
  
  //prevEnd = (left Vertex, right Vertex)
  void drawLightningSegment(float initialAngle, float width, const LightningNode& root, const std::pair<sf::Vector2f, sf::Vector2f> & prevEnd) {
    float currentAngle = initialAngle+root.m_angle;
    float perpAngle    = currentAngle + 90;
    auto dir  = sf::Vector2f(cos(currentAngle * PI/180.f), sin(currentAngle * PI/180.f));
    auto perp = sf::Vector2f(cos(perpAngle * PI/180.f), sin(perpAngle * PI/180.f)) * width;
    
    Vector2f beginPos;
    if (root.m_angle >= 0) { //we're going to the left
      beginPos = prevEnd.second; //base our line on the left vertex
      perp = -perp; //grow to the right
    } else {
      beginPos = prevEnd.first; //base on the right vertex and grow to the left
    }

    m_vertices.append(sf::Vertex( beginPos, sf::Vector2f(0,  0) ));
    m_vertices.append(sf::Vertex( beginPos + perp, sf::Vector2f(32, 1) ));

    auto rightEdge =  beginPos + dir*root.m_length;
    auto leftEdge = beginPos + perp + dir*root.m_length;
    
    const std::pair<sf::Vector2f, sf::Vector2f> currentEnd(leftEdge, rightEdge);
    m_vertices.append(sf::Vertex( leftEdge, sf::Vector2f(32, 0) ));
    m_vertices.append(sf::Vertex( rightEdge, sf::Vector2f(0,  1) ));
    
    // decide on the widths of the children
    float width1 = width, width2 = width;
    if (root.m_right != NULL && root.m_left) {
      const float alpha =  root.m_right->m_angle;
      const float beta =  -root.m_left->m_angle;
      const float gamma = 180.0f - (alpha + beta);
      cout << "a=" <<alpha<< " b=" << beta << " g=" << gamma <<endl;
      width1 = width * sin(90 - beta ) / sin(gamma);
      width2 = width * sin(90 - alpha) / sin(gamma);
    }
    
    // build the joint
    if (root.m_right != NULL && root.m_left) {
      auto leftVert  = sf::Vertex(currentEnd.first, sf::Vector(0, 0));
      auto rightVert = sf::Vertex(currentEnd.first, sf::Vector(32, 0));
    }
    
    if (root.m_right != NULL) drawLightningSegment(currentAngle, width2, *root.m_right, currentEnd);
    if (root.m_left  != NULL) drawLightningSegment(currentAngle, width1, *root.m_left, currentEnd);
  }

  sf::Texture     m_texture;
  sf::VertexArray m_vertices;
};

struct SoundData {
  sf::String  name;
  std::string filename;
};

list<SoundData> readJsonFile(const char* filename) {
  Json::Value root;
  Json::Reader reader;
  
  ifstream ifs(filename);
  
  bool success = reader.parse(ifs, root, false);
  if(!success) throw "Error reading json";
  
  vector<SoundData> result;
  for(auto& sound : root["alphabet"]) {
    std::string name = sound["name"].asString();
    std::string file = sound["file"].asString();
    result.push_back( {sf::String::fromUtf8(name.begin(), name.end()), file} ) ;
  }
  
  random_shuffle(result.begin(), result.end());
  return list<SoundData>(result.begin(), result.end());
}

int main() 
{
  sf::RenderWindow window(sf::VideoMode(800, 600), "Nightletters");
  window.setFramerateLimit(60);

  Nightsky nightsky(sf::Vector2i(800, 600));
  Lightning lightning;
  //text stuff
  sf::ComplexFont mf;
  mf.loadFromFile("amiri-regular.ttf");
  sf::String inputStr;
  StrokedText text, inputDisplay;
  text.setCharacterSize (50);
  text.setColor(sf::Color::Black, sf::Color(100,100,100));
  text.setPosition(sf::Vector2f(50,50));
  text.setFont(mf);
  
  inputDisplay.setCharacterSize (50);
  inputDisplay.setColor(sf::Color(100,100,100), sf::Color::Black);
  inputDisplay.setPosition(sf::Vector2f(200,200));
  inputDisplay.setFont(mf);
  
  
  list<SoundData> sounds(readJsonFile("arabic.json"));
  
  text.setString(sounds.front().name);
  
  sf::SoundBuffer buffer;
  if (!buffer.loadFromFile(sounds.front().filename))
    throw("failed to load sound");

  sf::Sound sound;
  sound.setBuffer(buffer);
  sound.play();
  
  // The main loop - ends as soon as the window is closed
  while (window.isOpen())
  {
    if (inputStr == sounds.front().name) {
      inputStr.clear();
      inputDisplay.setString(inputStr);
	  SoundData tmp = sounds.front();
	  sounds.push_back(tmp);
	  sounds.pop_front();
	  std::cout << sound.getStatus() << std::endl;
	  sound.stop();
	      std::cout << "playing:" << sounds.front().filename << std::endl;
		  sound.resetBuffer();
		  if (!buffer.loadFromFile(sounds.front().filename))
			throw("failed to load sound");
		  
		  sound.setBuffer(buffer);
		  sound.play();
		  text.setString(sounds.front().name);
    }
    // Event processing
    sf::Event event;
    while (window.pollEvent(event))
    {
	// Request for closing the window
	if (event.type == sf::Event::Closed) {
	    window.close();
        } else if (event.type == sf::Event::TextEntered) {
          if (event.text.unicode == '\b') {
            if (inputStr.getSize() <= 0) continue;
            inputStr.erase(inputStr.getSize() - 1, 1);
          } else {
            inputStr += event.text.unicode;
          }
          inputDisplay.setString(inputStr);
        }
    }
    
    nightsky.update();
    
    // Clear the whole window before rendering a new frame
    window.clear(sf::Color(255,255,255));
    
    // Draw some graphical entities
    window.draw(nightsky);
    window.draw(text);
    window.draw(lightning);
    window.draw(inputDisplay);
    
    // End the current frame and display its contents on screen
    window.display();
  }
}
