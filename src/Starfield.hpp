#ifndef __STARFIELD_HPP__
#define __STARFIELD_HPP__
#include <iostream>
template <typename T>
inline T clamp(const T& n, const T& lower, const T& upper) {
    return std::max(lower, std::min(n, upper));
}

class Star : public sf::Sprite {
public:
    
    Star() : m_startTime(0), m_lifeSpan(365*24*60*60)
    {
        int scale = rand() % 100;
        setCycle(scale * 0.05 / 100, 3 * (rand()%2000) / 2000.0f, (rand() % 314) / 100.0f, (10 + rand() % 40) / 100.0f);
        setRotation(rand() % 360);
    }
    
    void setTimes(float startTime, float lifeSpan)
    {
        m_startTime = startTime;
        m_lifeSpan = std::max(15.0f, lifeSpan);
    }
    
    void setCycle(float baseScale, float omega, float theta, float variance) {
        m_baseScale = baseScale;
        m_omega     = omega;
        m_theta     = theta;
        m_variance  = variance;
    }
  

    void update(float t) {
        float newScale = m_baseScale + m_variance * m_baseScale * sin(m_omega * t + m_theta);
        
        
        float dyingTransitionScale;
        if (t > m_startTime + m_lifeSpan)
        {
            dyingTransitionScale = 0; //star is already dead
        }
        else
        {
            const float lifeLeft = m_lifeSpan - (t-m_startTime);
            dyingTransitionScale = std::min(lifeLeft, 5.0f)/5.0f;
        }
        
        newScale *= dyingTransitionScale;
        setScale(newScale, newScale);
    }

    // solve omega*t1+theta1=omega*t2+theta2 for theta2 gives theta2=theta1+omega(t1-t2)
    void alignTheta(float t1Minust2)
    {
        m_theta = m_theta + m_omega * t1Minust2;
    }
    
    bool isDead(float t) const { return t > m_startTime + m_lifeSpan; }

private:
  float m_baseScale;
  float m_omega, m_theta, m_variance;
  
  // in seconds
  float m_startTime;
  float m_lifeSpan;
  bool m_dead;
};

class MovingStar : public Star
{
public:
    MovingStar(float initialX, float initialY, float finalX, float finalY)
    : initialPosition(initialX, initialY)
    , finalPosition(finalX, finalY)
    {
    }
    
    void update(float t)
    {
        float moveTransition = transitionFunction(clamp(t, 0.2f, 1.2f) - 0.2f);
        float appearTransition = transitionFunction(clamp(t, 0.0f, 0.2f) / 0.2f);
        
        
        
        const sf::Vector2f pos = (1 - moveTransition) * initialPosition + moveTransition * finalPosition;
        const sf::Uint8 opacity = appearTransition * 255;

        setPosition(pos);
        setColor(sf::Color(255, 255, 255, opacity));
        
        Star::update(t);
    }

private:
    
    // fα(x)=x^α/(x^α+(1−x)^α) for α=2 between 0 and 1. Function by Ansam
    float transitionFunction(float t)
    {
        const float t2 = t*t;
        const float nt = (1-t);
        const float nt2 = nt*nt;
        
        return t2/(t2+nt2);
    }
    
    const sf::Vector2f initialPosition, finalPosition;
};

class StationaryStars : public sf::Drawable {
public:
    StationaryStars(sf::Texture *starTex) 
    {
        m_starTex = starTex;
    }
    
    void generateStars(size_t n, const sf::Vector2i& windowSize)
    {
        m_t=0;
        m_stars.resize(n);
    
        for(auto& star : m_stars) 
        {
            star.setTexture(*m_starTex, true);
            star.setOrigin(star.getTextureRect().width/2, star.getTextureRect().height/2);
            star.setPosition(rand() % windowSize.x, rand() % windowSize.y);
            star.setTimes(m_t, rand() % (5*60));
        }
    }
  
    virtual void  draw(sf::RenderTarget &target, sf::RenderStates states) const 
    {
        for(auto& star : m_stars) 
        {
            target.draw(star, states);
        }
    }
  
    void update()
    {
        for(auto& star : m_stars)
        {
            star.update(m_t);
        }
        m_t+=0.017;
    }
    
    
    void addStars(const std::vector<MovingStar>& movingStars, float starTime)
    {
        std::vector<Star> newStars;
        newStars.reserve(m_stars.size() + newStars.size());
        
        for (auto& star : m_stars)
        {
            if (!star.isDead(m_t))
            {
                newStars.push_back(star);
            }
        }
        
        int numberOfStars = newStars.size();
        const int maxStars = 500;
        float factor = std::max(float(maxStars - numberOfStars) / maxStars, 0.0f);
        const float t1Minust2 = m_t - starTime;
        for(auto& star : movingStars)
        {
            Star newStar(star);
            newStar.alignTheta(t1Minust2);
            newStar.setTimes(m_t, factor * (rand() % (5*60)));
            
            
            newStars.push_back(newStar);
        }
        
        std::swap(m_stars, newStars);
    }

private:
    sf::Vector2i windowSize;
    sf::Texture* m_starTex;
    std::vector<Star> m_stars;
    float m_t;
};


#include "movingStars.hpp"

class Starfield : public sf::Drawable {
public:
    Starfield(size_t n, const char * starImage, const sf::Vector2i& windowSize)
    : m_movingStars(&m_texture)
    , m_stationaryStars(&m_texture)
    {
        m_texture.loadFromFile(starImage);
        m_stationaryStars.generateStars(n, windowSize);
    }

    virtual void  draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        target.draw(m_movingStars, states);
        target.draw(m_stationaryStars, states);
    }

    void update()
    {
        m_movingStars.update();
        m_stationaryStars.update();
    }

    void generateStars(sf::String str, sf::ComplexFont &mf, unsigned int characterSize, const sf::Vector2f& textPosition, const sf::Vector2i& windowSize)
    {
        m_stationaryStars.addStars(m_movingStars.getStars(), m_movingStars.getTime());
        m_movingStars.generateStars(str, mf, characterSize, textPosition, windowSize);
        
    }

private:
    MovingStars m_movingStars;
    StationaryStars m_stationaryStars;
    sf::Texture m_texture;
};

#endif // __STARFIELD_HPP__