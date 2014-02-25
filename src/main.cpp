#include <GL/glew.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Audio.hpp>
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

#include "Nightsky.hpp"
#include "StrokedText.hpp"
#include "lightning.hpp"
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
  srand(time(NULL));
  sf::RenderWindow window(sf::VideoMode(800, 600), "Nightletters");
  window.setFramerateLimit(60);
  sf::RenderTexture framebuffer;
  framebuffer.create(800,600);
  sf::Sprite lightningSprite(framebuffer.getTexture());

  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    throw std::runtime_error("Couldn't initialize GLEW");
  }  
  
  Nightsky nightsky(sf::Vector2i(800, 600));
  LightningBolt lightning(sf::Vector2f(500,0), sf::Vector2f(500,500), 4, sf::Vector2i(300,0));
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
	  sound.stop();
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
    lightning.update();
    // Clear the whole window before rendering a new frame
    window.clear(sf::Color(255,255,255));
    
    // Draw some graphical entities
    window.draw(nightsky);
    window.draw(text);
    framebuffer.clear(sf::Color(0,0,0,0));
    glBlendEquation(GL_MAX);
    framebuffer.draw(lightning);
    framebuffer.display();
    glBlendEquation(GL_FUNC_ADD);
    window.draw(lightningSprite);
    //window.draw(lightning);
    window.draw(inputDisplay);
    
    // End the current frame and display its contents on screen
    window.display();
  }
}
