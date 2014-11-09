#include <list>
#include <vector>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <algorithm>
#include <SFML/Audio.hpp>

struct SoundData {
  sf::String  name;
  std::string filename;
  std::vector<sf::String> acceptbaleAnswers;
};

class SoundManager {
public:
  void readJsonFile(const char* filename) {
    Json::Value root;
    Json::Reader reader;
    
    std::ifstream ifs(filename);
    
    bool success = reader.parse(ifs, root, false);
    if(!success) {
      std::cout << "Error reading json file: " << filename << std::endl;
      throw "Error reading json";
    }
    
    std::vector<SoundData> result;
    if(!root.isMember("alphabet")) {
      std::cout << "Doesn't contain alphabet" << filename << std::endl;
      throw "Doesn't contain alphabet";
    }
    
    for(auto& sound : root["alphabet"]) {
      if(!sound.isMember("name")  ||!sound.isMember("file")  ||!sound.isMember("accept")) {
        std::cout << "Doesn't contain name,file,accept" << filename << std::endl;
        throw "Doesn't contain name,file,accept";
      }
      
      std::string name = sound["name"].asString();
      std::string file = sound["file"].asString();
      sf::String sfml_name = sf::String::fromUtf8(name.begin(), name.end());
      std::vector<sf::String> acceptbaleAnswers;
      for(const Json::Value& answer: sound["accept"]) {
        std::string strAnswer = answer.asString();
        acceptbaleAnswers.push_back(sf::String::fromUtf8(strAnswer.begin(), strAnswer.end()));
      }
      if (acceptbaleAnswers.empty()) acceptbaleAnswers.push_back(sfml_name);
      
      result.push_back( {sfml_name, file, acceptbaleAnswers}) ;
    }
    
    std::random_shuffle(result.begin(), result.end());
    
    std::list<SoundData> resultList(result.begin(), result.end());
    std::swap(m_sounds, resultList);
  }
  
  sf::String getDisplayText() const {
   return m_sounds.front().name;
  }
  
  bool acceptableAnswer(sf::String str) const {
    for(const sf::String& answer: m_sounds.front().acceptbaleAnswers) {
      if(str == answer) return true;
    }
    
    return false;
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