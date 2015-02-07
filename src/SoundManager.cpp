#include <jsoncpp/json/json.h>
#include <fstream>
#include <algorithm>
#include "SoundManager.hpp"

SoundManager::AnswerCheckResult SoundManager::checkAnswer(const sf::String& str)
{
    if (str.getSize() == 0)
    {
        return RESULT_PARTIAL;
    }

    for(const sf::String& answer : m_sounds.front().acceptbaleAnswers)
    {
        if(str.getSize() <= answer.getSize() && matchingPrefixes(str, answer))
        {
            if (str.getSize() == answer.getSize())
            {
                m_userDifficulty += m_sounds.front().difficulty;
                return RESULT_RIGHT;
            }
            return RESULT_PARTIAL;
        }
    }

    if (m_userDifficulty > 1)
    {   // can't drop below 1!
        m_userDifficulty -= m_sounds.front().difficulty;
    }
    return RESULT_WRONG;
}

std::string SoundManager::readJsonFile(const char* filename)
{
    m_userDifficulty = 1;
    m_filename = filename;

    Json::Value root;
    Json::Reader reader;

    std::ifstream ifs(filename);

    bool success = reader.parse(ifs, root, false);
    if(!success)
    {
        std::cout << "Error reading json file: " << filename << std::endl;
        throw "Error reading json";
    }

    std::vector<SoundData> result;
    if(!root.isMember("alphabet"))
    {
        std::cout << "Doesn't contain alphabet" << filename << std::endl;
        throw "Doesn't contain alphabet";
    }

    for(auto& sound : root["alphabet"])
    {
        if(!sound.isMember("name")  ||!sound.isMember("file"))
        {
            std::cout << "Doesn't contain name,file" << filename << std::endl;
            throw "Doesn't contain name,file";
        }

        std::string name = sound["name"].asString();
        std::string file = sound["file"].asString();
        sf::String sfml_name = sf::String::fromUtf8(name.begin(), name.end());
        std::vector<sf::String> acceptbaleAnswers;

        if (sound.isMember("accept"))
        {
            for(const Json::Value& answer: sound["accept"])
            {
                std::string strAnswer = answer.asString();
                acceptbaleAnswers.push_back(sf::String::fromUtf8(strAnswer.begin(), strAnswer.end()));
            }
        }
        if (acceptbaleAnswers.empty()) acceptbaleAnswers.push_back(sfml_name);

        unsigned int difficulty = 1;
        if (sound.isMember("difficulty"))
        {
            difficulty = sound["difficulty"].asUInt();
        }

        result.push_back( {sfml_name, file, acceptbaleAnswers, difficulty} );
    }

    std::random_shuffle(result.begin(), result.end());

    std::list<SoundData> resultList(result.begin(), result.end());
    std::swap(m_sounds, resultList);

    next(); //to avoid getting something too difficult first

    return root.isMember("fontFile") ? root["fontFile"].asString() : "LiberationSerif-Regular.ttf";
}

sf::String SoundManager::getDisplayText() const {
    return m_sounds.front().name;
}

/*static*/
bool SoundManager::matchingPrefixes(const sf::String& str1, const sf::String& str2)
{
    size_t index = 0;
    bool equal = true;
    while ( str1[index] != 0 && str2[index] != 0)
    {
        equal = equal && (str1[index] == str2[index]);
        index++;
    }
    return equal;
}

void SoundManager::playSound()
{
    m_soundPlayer.stop();
    m_soundPlayer.resetBuffer();
    if (!m_buffer.loadFromFile(m_sounds.front().filename))
        throw("failed to load sound");

    m_soundPlayer.setBuffer(m_buffer);
    m_soundPlayer.play();
}

void SoundManager::next()
{
    do
    {
        SoundData tmp = m_sounds.front();
        m_sounds.push_back(tmp);
        m_sounds.pop_front();
        std::cout << "userDifficulty: " << m_userDifficulty << " wordDifficulty: " << m_sounds.front().difficulty << std::endl;
        //std::cout << " word: "  << m_sounds.front().name.toUtf8()
    } while (m_sounds.front().difficulty > m_userDifficulty);
}


const std::string SoundManager::getLoadedFile() const
{
    return m_filename;
}
