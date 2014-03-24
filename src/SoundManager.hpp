#include <list>
#include <vector>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <algorithm>
#include <SFML/Audio.hpp>

struct SoundData {
  sf::String  name;
  std::string filename;
};

class SoundManager {
public:
  void readJsonFile(const char* filename) {
    Json::Value root;
    Json::Reader reader;
    
    std::ifstream ifs(filename);
    
    bool success = reader.parse(ifs, root, false);
    if(!success) throw "Error reading json";
    
    std::vector<SoundData> result;
    for(auto& sound : root["alphabet"]) {
      std::string name = sound["name"].asString();
      std::string file = sound["file"].asString();
      result.push_back( {sf::String::fromUtf8(name.begin(), name.end()), file} ) ;
    }
    
    std::random_shuffle(result.begin(), result.end());
    
    std::list<SoundData> resultList(result.begin(), result.end());
    std::swap(m_sounds, resultList);
  }
  
  sf::String getDisplayText() const {
   return m_sounds.front().name;
  }
  
  void playSound() {
    m_soundPlayer.stop();
    m_soundPlayer.resetBuffer();
    if (!m_buffer.loadFromFile(m_sounds.front().filename))
    throw("failed to load sound");
    
    m_soundPlayer.setBuffer(m_buffer);
    m_soundPlayer.play();
  }
  
  void next() {
    SoundData tmp = m_sounds.front();
	  m_sounds.push_back(tmp);
	  m_sounds.pop_front();
  }
  
private:
  std::list<SoundData> m_sounds;
  sf::Sound            m_soundPlayer;
  sf::SoundBuffer      m_buffer;
};