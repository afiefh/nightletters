#include <GL/glew.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <list>

#include "Nightsky.hpp"
#include "StrokedText.hpp"
#include "utilities.hpp"
#include "SoundManager.hpp"
#include "Action.hpp"
#include "Actions.hpp"
#include <cwchar>
#include <iostream>
#include <cassert>
#include <stdio.h>
#include <cmath>
#include <functional>
#include "lightning.hpp"
#include "tree.hpp"
#include "menu.hpp"

void initializeGlew() {
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    throw std::runtime_error("Couldn't initialize GLEW");
  }
}

class ActionList : public std::list<Action*> {
public:
  void update(const sf::Time &dt) {
    auto it = this->begin();
    
    while(it != this->end()) {
      (*it)->update(dt);
      if ((*it)->isFinished()) { 
        (*it)->onEnd();
        it = this->erase(it); //returns the next element
        continue; 
      }
      if ((*it)->isBlocking()) break;
      ++it;
    }
    
  }
};

#include <jsoncpp/json/json.h>
#include <fstream>

//language loader must be a functor that takes exactly one string parameter which is the data file
template<typename LanguageLoader>
void populateMenu(const char* languageFile, LanguageLoader loader, Menu &menu)
{
    Json::Value root;
    Json::Reader reader;

    std::ifstream ifs(languageFile);

    bool success = reader.parse(ifs, root, false);
    if(!success) {
        std::cout << "Error reading json file: " << languageFile << std::endl;
        throw "Error reading json";
    }

    std::vector<SoundData> result;

    bool first = true;
    for(auto& language : root) {
        if(!language.isMember("icon")  || !language.isMember("data")) {
            std::cout << "Doesn't contain name icon, data" << languageFile << std::endl;
            throw "Doesn't contain icon, data";
        }
        
        std::string icon = language["icon"].asString();
        std::string data = language["data"].asString();

        if (first)
            loader(data);
        first = false;
        
        menu.addSubmenu(icon).setFunction(std::bind(loader, std::move(data)));
    }
    
}

int main() 
{
  const sf::Vector2i windowSize(800, 600);
  ActionList actionList;
  srand(time(NULL));
  sf::RenderWindow window(sf::VideoMode(windowSize.x, windowSize.y), "Nightletters");
  window.setFramerateLimit(60);
  initializeGlew(); //has to be done after OpenGL context was created
  
  sf::RenderTexture framebuffer;
  framebuffer.create(windowSize.x, windowSize.y);
  sf::Sprite lightningSprite(framebuffer.getTexture());
  
  Nightsky nightsky(windowSize);
  FlashLightning lightning(4, sf::Vector2i(800,600), sf::seconds(1.0f), windowSize, sf::Vector2f(200,0));
  
  SoftBody softBody("../graphic/tree_leaves.png");
  softBody.move(476, 58);
  softBody.update();
  
  //text stuff
  sf::ComplexFont mf;
  mf.loadFromFile("amiri-regular.ttf");
  sf::String inputStr;
  StrokedText text, inputDisplay;
  text.setCharacterSize (50);
  text.setColor(sf::Color::Black, sf::Color(100,100,100));
  text.setPosition(sf::Vector2f(50,50));
  text.setFont(mf);
  
  SoundManager soundManager;
  
  //the menu
  Menu menu("../graphic/icon-globe.png");
  menu.setPosition (windowSize.x * 0.9, windowSize.y * 0.9);
  populateMenu("languages.json", [&soundManager, &text](std::string& dataFile) {
      if (dataFile == soundManager.getLoadedFile())
      {
          std::cout << "File " << dataFile << " already loaded" << std::endl;
          return;
      }
      std::cout << "Loading " << dataFile << std::endl;
      soundManager.readJsonFile(dataFile.c_str());
      soundManager.playSound();
      text.setString(soundManager.getDisplayText());
      std::wstring s(soundManager.getDisplayText().toWideString());
      std::wcout << L"Start with: " << s << std::endl;
    }, menu);
  
  inputDisplay.setCharacterSize (50);
  inputDisplay.setColor(sf::Color(100,100,100), sf::Color::Black);
  inputDisplay.setPosition(sf::Vector2f(200,200));
  inputDisplay.setFont(mf);
  
/*  
  soundManager.readJsonFile("english.json");
  
  soundManager.playSound();
  text.setString(soundManager.getDisplayText());
*/
  
  bool acceptInput(true);
  sf::Clock clock;
  
  FadeText inputDisplayFadeIn(&inputDisplay, sf::Color(100,100,100,0), sf::Color(0,0,0,0), //initial
                                              sf::Color(100,100,100), sf::Color::Black,     //final
                                              sf::seconds(0.5));
  FadeText inputDisplayFadeOut(&inputDisplay, sf::Color(100,100,100), sf::Color::Black,      //initial
                                             sf::Color(100,100,100,0), sf::Color(0,0,0,0), //final
                                             sf::seconds(0.5));
  FadeText textDisplayFadeIn(&text,         sf::Color(100,100,100,0), sf::Color(0,0,0,0), //initial
                                             sf::Color(100,100,100), sf::Color::Black,     //final
                                             sf::seconds(0.5));
  FadeText textDisplayFadeOut(&text,          sf::Color(100,100,100), sf::Color::Black,      //initial
                                             sf::Color(100,100,100,0), sf::Color(0,0,0,0),  //final
                                             sf::seconds(0.5));
                                             
  SimpleAction nextLetter(
    [&soundManager, &text, &inputStr, &inputDisplay](const sf::Time& /*dt*/)->bool 
    {
      soundManager.next();
      text.setString(soundManager.getDisplayText());
      soundManager.playSound();
      
      inputStr.clear();
      inputDisplay.setString(inputStr);

      
      return true; //finished
    }
  );
  
  // The main loop - ends as soon as the window is closed
  while (window.isOpen())
  {
    sf::Time dt = clock.restart();
    if (soundManager.acceptableAnswer(inputStr) && acceptInput) {
      acceptInput = false;
      lightning.onStart();
      
      textDisplayFadeIn.restart();
      textDisplayFadeOut.restart();
      inputDisplayFadeIn.restart();
      inputDisplayFadeOut.restart();
      
      actionList.push_back(&inputDisplayFadeOut);
      actionList.push_back(&textDisplayFadeOut);
      actionList.push_back(&lightning);
      actionList.push_back(&nextLetter);
      actionList.push_back(&inputDisplayFadeIn);
      actionList.push_back(&textDisplayFadeIn);
    }
    
    // Event processing
    sf::Event event;
    while (window.pollEvent(event))
    {
      // Request for closing the window
      if (event.type == sf::Event::Closed) {
          window.close();
      } else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                menu.clicked(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
            }
      } else if (event.type == sf::Event::TextEntered && acceptInput) {
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
    std::pair<float, float> windPosition = nightsky.getBreeze().getStartAndEnd();
    softBody.velocityRight(windPosition.first - 476, windPosition.second - 476, 0, 200, 5);
    softBody.update();
    menu.update();
    // Clear the whole window before rendering a new frame
    window.clear(sf::Color(255,255,255));
    
    // Draw some graphical entities
    window.draw(nightsky);
    window.draw(text);
    actionList.update(dt);
    //lightning.update(0.0f); //TODO: need to take dt into account at some point
    acceptInput = actionList.empty();
    framebuffer.clear(sf::Color(0,0,0,0));
    /*
    glBlendEquation(GL_MAX);
    framebuffer.draw(lightning);
    framebuffer.display();
    glBlendEquation(GL_FUNC_ADD);
    window.draw(lightningSprite);
    */
    window.draw(lightning);
    /*
    if (lightning.isFinished() == false) {
      lightning.update(0.0f); //TODO: need to take dt into account at some point
      acceptInput = lightning.isFinished() ? true : false;
      framebuffer.clear(sf::Color(0,0,0,0));
      glBlendEquation(GL_MAX);
      framebuffer.draw(lightning);
      framebuffer.display();
      glBlendEquation(GL_FUNC_ADD);
      window.draw(lightningSprite);
    }*/
    
    window.draw(inputDisplay);
    window.draw(softBody);
    window.draw(menu);
    // End the current frame and display its contents on screen
    window.display();
  }
}
