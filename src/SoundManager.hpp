#include <list>
#include <vector>
#include <string>
#include <SFML/Audio.hpp>

class SoundManager {
public:
    enum AnswerCheckResult {
      RESULT_RIGHT,
      RESULT_WRONG,
      RESULT_PARTIAL
    };

    std::string readJsonFile(const char* filename);
  
    sf::String getDisplayText() const;

    AnswerCheckResult checkAnswer(const sf::String& str) const;
  
    bool acceptableAnswer(sf::String str) const;
    
    static bool matchingPrefixes(const sf::String& str1, const sf::String& str2);
    
    bool isAcceptableSubstring(sf::String& str) const;
  
    void playSound();
  
    void next();
  
    const std::string getLoadedFile() const;
  
private:
    struct SoundData {
        sf::String  name;
        std::string filename;
        std::vector<sf::String> acceptbaleAnswers;
    };

    std::list<SoundData> m_sounds;
    sf::Sound            m_soundPlayer;
    sf::SoundBuffer      m_buffer;
    std::string          m_filename;
};