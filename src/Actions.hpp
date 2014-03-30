class FadeText : public Action {
public:
  FadeText(StrokedText* text,
           sf::Color initialForeground, sf::Color initialBackground, 
           sf::Color finalForeground, sf::Color finalBackground,
           sf::Time duration) 
               : m_text(text),
                 m_initialForeground(initialForeground), 
                 m_initialBackground(initialBackground),
                 m_finalForeground(finalForeground),
                 m_finalBackground(finalBackground),
                 m_duration(duration),
                 m_time(sf::seconds(0))
  { }
  
  static sf::Color mix(const sf::Color& c1, const sf::Color& c2, float p) {
    return mult(c1, p) + mult(c2, 1-p);
  }
  
  static sf::Color mult(const sf::Color& c, float f) {
    return sf::Color(c.r*f, c.g*f, c.b*f, c.a*f);
  }
  
  virtual void update(const sf::Time &dt) {
    const float percentage = m_time.asSeconds() / m_duration.asSeconds();
    m_text->setColor(mix(m_finalForeground, m_initialForeground, percentage),
                     mix(m_finalBackground, m_initialBackground, percentage));
    m_time += dt;
    //std::cout << this << " Percentage: " << percentage << std::endl;
  }
  
  virtual void onStart() {
    m_text->setColor(m_initialForeground, m_initialBackground);
  }
  virtual void onEnd() {
    m_text->setColor(m_finalForeground, m_finalBackground);
  }
  virtual bool isBlocking() const { return false; }
  virtual bool isFinished() const { return m_time > m_duration; }
  void restart() {  m_time = sf::seconds(0); }
  
private:
  StrokedText * const m_text;
  const sf::Color     m_initialForeground;
  const sf::Color     m_initialBackground;
  const sf::Color     m_finalForeground;
  const sf::Color     m_finalBackground;
  const sf::Time      m_duration;
  sf::Time            m_time;
};