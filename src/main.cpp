#include <GL/glew.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <list>

#include "Nightsky.hpp"
#include "StrokedText.hpp"
#include "lightning.hpp"
#include "utilities.hpp"
#include "SoundManager.hpp"
#include "Action.hpp"
#include <cwchar>
#include <iostream>
#include <cassert>
#include <stdio.h>
#include <cmath>
#include <functional>

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

class SimpleAction : public Action {
public:
  SimpleAction(std::function<bool(const sf::Time &dt)> func) : m_func(func), m_finished(false) {}
  virtual void update(const sf::Time &dt) { m_finished = m_func(dt);}
  virtual void onStart() {};
  virtual void onEnd() {};
  virtual bool isBlocking() const { return false; }
  virtual bool isFinished() const { return m_finished; }
private:
  std::function<bool(const sf::Time &dt)> m_func;
  bool                                    m_finished;
};

int main() 
{
  ActionList actionList;
  srand(time(NULL));
  sf::RenderWindow window(sf::VideoMode(800, 600), "Nightletters");
  window.setFramerateLimit(60);
  initializeGlew(); //has to be done after OpenGL context was created
  
  sf::RenderTexture framebuffer;
  framebuffer.create(800,600);
  sf::Sprite lightningSprite(framebuffer.getTexture());
  
  
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
  
  SoundManager soundManager;
  soundManager.readJsonFile("arabic.json");
  
  soundManager.playSound();
  text.setString(soundManager.getDisplayText());
  
  bool acceptInput(true);
  sf::Clock clock;
  
  SimpleAction nextLetter(
  [&soundManager, &text](const sf::Time& /*dt*/)->bool 
  {
    soundManager.next();
    text.setString(soundManager.getDisplayText());
    soundManager.playSound();

    return true; //finished
  });
  // The main loop - ends as soon as the window is closed
  while (window.isOpen())
  {
    sf::Time dt = clock.restart();
    if (inputStr == soundManager.getDisplayText()) {
      inputStr.clear();
	    inputDisplay.setString(inputStr);
      text.setString("");
      acceptInput = false;
      lightning.start();
      actionList.push_back(&lightning);
      actionList.push_back(&nextLetter);
      //TODO: will have to think of something for these
    }
    // Event processing
    sf::Event event;
    while (window.pollEvent(event))
    {
      // Request for closing the window
      if (event.type == sf::Event::Closed) {
          window.close();
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
    // Clear the whole window before rendering a new frame
    window.clear(sf::Color(255,255,255));
    
    // Draw some graphical entities
    window.draw(nightsky);
    window.draw(text);
    actionList.update(dt);
    //lightning.update(0.0f); //TODO: need to take dt into account at some point
    acceptInput = actionList.empty();
    framebuffer.clear(sf::Color(0,0,0,0));
    glBlendEquation(GL_MAX);
    framebuffer.draw(lightning);
    framebuffer.display();
    glBlendEquation(GL_FUNC_ADD);
    window.draw(lightningSprite);
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
    
    // End the current frame and display its contents on screen
    window.display();
  }
}
