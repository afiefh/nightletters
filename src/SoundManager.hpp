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

    AnswerCheckResult checkAnswer(const sf::String& str);
  
    void playSound();
  
    void next();
  
    const std::string getLoadedFile() const;
  
private:
    static bool matchingPrefixes(const sf::String& str1, const sf::String& str2);

    struct SoundData {
        sf::String  name;
        std::string filename;
        std::vector<sf::String> acceptbaleAnswers;
        unsigned int difficulty;
        unsigned int lastAsked;
    };

    std::list<SoundData> m_sounds;
    std::list<SoundData> m_mistakeRepeats;
    sf::Sound            m_soundPlayer;
    sf::SoundBuffer      m_buffer;
    std::string          m_filename;
    unsigned int         m_userDifficulty;
    unsigned int         m_timestamp;
    SoundData            m_currentSound;
};
