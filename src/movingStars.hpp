#include "ComplexText.hpp"
#include "ComplexFont.hpp"

const unsigned int renderFactor = 4;

class MovingStars : public sf::Drawable
{
public:
    MovingStars(sf::Texture* texture)
        : m_texture(texture)
        , m_t(0)
    {
        m_texture = texture;
    }

    void generateStars(sf::String str, sf::ComplexFont &mf, unsigned int characterSize, const sf::Vector2f& textPosition, const sf::Vector2i& windowSize)
    {
        m_stars.clear();
        sf::ComplexText text(str, mf, characterSize / renderFactor);
        text.setColor(sf::Color::White);
        sf::FloatRect rect = text.getLocalBounds();
        text.setPosition(-rect.left, - rect.top);
        m_framebuffer.create(rect.width, rect.height);
        m_framebuffer.clear();
        m_framebuffer.draw(text);
        m_framebuffer.display();
        
        sf::Image img(m_framebuffer.getTexture().copyToImage());
        
        const sf::Vector2u imgSize = img.getSize();

        const sf::Uint8* pixelPtr = img.getPixelsPtr();
        for(unsigned int i = 0; i < imgSize.y; ++i)
        {
            for(unsigned int j = 0; j < imgSize.x; ++j)
            {
                uint16_t r = *(pixelPtr + (i * imgSize.x +  j) * 4);
                uint16_t g = *(pixelPtr + (i * imgSize.x +  j) * 4 + 1);
                uint16_t b = *(pixelPtr + (i * imgSize.x +  j) * 4 + 2);
                if ((r+g+b) / 3 > 20)
                {
                    const float initialX = rect.left * renderFactor + textPosition.x + j * renderFactor + renderFactor/2;
                    const float initialY = rect.top * renderFactor  + textPosition.y + i * renderFactor + renderFactor/2;
                    MovingStar newStar(initialX, initialY, rand() % windowSize.x, rand() % windowSize.y);
                    newStar.setTexture(*m_texture, true);
                    newStar.setOrigin(newStar.getTextureRect().width/2, newStar.getTextureRect().height/2);
                    
                    m_stars.push_back(newStar);
                }
                
            }
        }
        m_t = 0;
    }

    void update()
    {
        for(auto& star: m_stars)
        {
            int scale = rand() % 100;
            star.update(m_t);
        }
        //m_t = 1;
        m_t += 0.01;
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
    {
        for(auto& star : m_stars) {
            target.draw(star, states);
        }
    }
    
    std::vector<MovingStar> getStars() { return std::move(m_stars); }
    float getTime() const { return m_t; }
    
    
private:
    std::vector<MovingStar> m_stars;
    sf::RenderTexture       m_framebuffer;
    sf::Texture           * m_texture;
    float                   m_t;
};