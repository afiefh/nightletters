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

    AnswerCheckResult checkAnswer(const sf::String& str);

    void next();

    void playSound();

    sf::String getDisplayText() const { return m_currentSound.name; }
    std::string getImageFilename() const { return m_currentSound.imageFilename; }
    const std::string getLoadedFile() const { return m_filename; }

private:
    static bool matchingPrefixes(const sf::String& str1, const sf::String& str2);

    struct SoundData {
        sf::String  name;
        std::string filename;
        std::string imageFilename;
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
