
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
	window.draw(inputDisplay);
    
    // End the current frame and display its contents on screen
    window.display();
  }
}
