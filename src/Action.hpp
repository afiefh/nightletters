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

#endif