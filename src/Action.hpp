#ifndef __ACTION_HPP__
#define __ACTION_HPP__

namespace sf {
  class Time;
}

class Action {
public:
  virtual void update(const sf::Time &dt) = 0;
  virtual void onStart()                  = 0;
  virtual void onEnd()                    = 0;
  virtual bool isBlocking() const         = 0;
  virtual bool isFinished() const         = 0;
};

#endif