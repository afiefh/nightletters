
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

#include <jsoncpp/json/json.h>
#include <fstream>
#include <algorithm>
using namespace std;

struct SoundData {
  sf::String  name;
  std::string filename;
};

vector<SoundData> readJsonFile(const char* filename) {
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
  return result;
}

int main() 
{
  sf::RenderWindow window(sf::VideoMode(800, 600), "Nightletters");
  window.setFramerateLimit(60);

  Nightsky nightsky(sf::Vector2i(800, 600));
 
  //text stuff
  sf::Font mf;
  mf.loadFromFile("amiri-regular.ttf");
  sf::String s;
  StrokedText text;
  text.setCharacterSize (50);
  text.setColor(sf::Color::Black, sf::Color(100,100,100));
  text.setPosition(sf::Vector2f(50,50));
  text.setString(s);
  text.setFont(mf);
  
  
  vector<SoundData> sounds(readJsonFile("arabic.json"));
  random_shuffle(sounds.begin(), sounds.end());
  
  s = sounds[0].name;
  text.setString(s);
  
  
  
  sf::SoundBuffer buffer;
  if (!buffer.loadFromFile(sounds[0].filename))
    throw("failed to load sound");

  sf::Sound sound;
  sound.setBuffer(buffer);
  sound.play();
  
  // The main loop - ends as soon as the window is closed
  while (window.isOpen())
  {
    if (s == sf::String(L"عفيف")) {
      s.clear();
      text.setString(s);
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
            if (s.getSize() <= 0) continue;
            s.erase(s.getSize() - 1, 1);
          } else {
            s += event.text.unicode;
          }
          text.setString(s);
        }
    }
    
    nightsky.update();
    
    // Clear the whole window before rendering a new frame
    window.clear(sf::Color(255,255,255));
    
    // Draw some graphical entities
    window.draw(nightsky);
    window.draw(text);
    
    // End the current frame and display its contents on screen
    window.display();
  }
}
