
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

const float PI = 3.14159265359f;
class Lightning : public sf::Drawable, public sf::Transformable{
public:
  Lightning() : m_vertices(sf::Quads){
    m_texture.loadFromFile("../graphic/lightning2.png");
    m_bolt.push_back(sf::Vector2f(600,100));
    m_bolt.push_back(sf::Vector2f(600,600));
    generateLightning();
  }

  void generateLightning() {
    srand(time(NULL));
    vector<sf::Vector2f> tmpBolt;
    for (size_t subdivision=0;subdivision<7;subdivision++) {
      tmpBolt.clear();
      tmpBolt.push_back(m_bolt[0]);
      for (size_t i=1; i<m_bolt.size();i++) {
        //for any subsequent point
        float division = (rand()%20 + 40) / 100.0f;
        sf::Vector2f direction = m_bolt[i] - m_bolt[i-1];
        sf::Vector2f perp = getPerpendicularDir(direction);
        float length = getLength(direction);
        sf::Vector2f midpoint = m_bolt[i-1] + division*(m_bolt[i] - m_bolt[i-1]) + (rand()%2-1) * 0.35f*length*perp;
        tmpBolt.push_back(midpoint);
        tmpBolt.push_back(m_bolt[i]);
      }
      std::swap(m_bolt, tmpBolt);
    }
    
    for (size_t i=1; i<m_bolt.size();i++) {
      const float width = 80;
      
      sf::Vector2f dir = m_bolt[i] - m_bolt[i-1];
      dir = dir / getLength(dir) * (width/2); //we need with for the length of the cap
      sf::Vector2f perp = getPerpendicularDir(dir)*width; //twice as much as the length
      
      //pre cap
      m_vertices.append(sf::Vertex( m_bolt[i-1] + perp, sf::Vector2f(0,0) ));
      m_vertices.append(sf::Vertex( m_bolt[i-1] - perp, sf::Vector2f(0,128) ));
      m_vertices.append(sf::Vertex( m_bolt[i-1] - dir - perp, sf::Vector2f(64,128) ));
      m_vertices.append(sf::Vertex( m_bolt[i-1] - dir + perp, sf::Vector2f(64,0) ));
      
      // middle
      m_vertices.append(sf::Vertex( m_bolt[i-1] + perp, sf::Vector2f(0,0) ));
      m_vertices.append(sf::Vertex( m_bolt[i-1] - perp, sf::Vector2f(0,128) ));
      m_vertices.append(sf::Vertex( m_bolt[ i ] - perp, sf::Vector2f(1,128) ));
      m_vertices.append(sf::Vertex( m_bolt[ i ] + perp, sf::Vector2f(1,0) ));
      
      m_vertices.append(sf::Vertex( m_bolt[ i ] + perp, sf::Vector2f(0,0) ));
      m_vertices.append(sf::Vertex( m_bolt[ i ] - perp, sf::Vector2f(0,128) ));
      m_vertices.append(sf::Vertex( m_bolt[ i ] + dir - perp, sf::Vector2f(64,128) ));
      m_vertices.append(sf::Vertex( m_bolt[ i ] + dir + perp, sf::Vector2f(64,0) ));
    }
  }
  
  void draw(sf::RenderTarget& target, sf::RenderStates states) const
  {
    states.transform *= getTransform();
    states.texture    = &m_texture;
    target.draw(m_vertices, states);
  }

private:
  sf::Vector2f getPerpendicularDir(sf::Vector2f dir) const { //TODO: put it in a common place so wind and lightning can use it
    float length = sqrt(dir.x * dir.x + dir.y * dir.y);
    sf::Vector2f perpDir(-dir.y / length, dir.x / length);
    
    return perpDir;
  }
  float getLength(sf::Vector2f vec) const {
    return sqrt(vec.x * vec.x + vec.y * vec.y);
  }
  
  sf::Texture           m_texture;
  vector<sf::Vector2f>  m_bolt;
  sf::VertexArray       m_vertices;
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
  sf::RenderTexture framebuffer;
  framebuffer.create(800,600);
  
  
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
    framebuffer.clear(sf::Color(0,0,0,0));
    framebuffer.draw(lightning, sf::BlendMax);
    sf::Sprite lightningSprite(framebuffer.getTexture());
    window.draw(lightningSprite);
    window.draw(inputDisplay);
    
    // End the current frame and display its contents on screen
    window.display();
  }
}
