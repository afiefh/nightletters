#ifndef __MENU_HPP__
#define __MENU_HPP__

#include <SFML/Graphics.hpp>
class Menu : public sf::Drawable, public sf::Transformable 
{
public:
    Menu(const std::string& iconPath) : m_isOpen(false), m_transition(0.0f)
    {
        std::cout << "loading icon: " << iconPath << std::endl;
        bool success = texture.loadFromFile(iconPath.c_str());
        std::cout << "load successful: " << success << std::endl;
        if (!success)
        {
            throw std::string("Failed to load texture: ") + iconPath;
        }
        icon.setTexture(texture, true);
    }
    
    Menu(const Menu& other) : texture(other.texture), subMenus(other.subMenus), actionFunc(other.actionFunc), m_isOpen(other.m_isOpen)
    {
        icon.setTexture(texture,true);
        icon.setColor(other.icon.getColor());
    }
    
    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        states.transform *= getTransform();
        target.draw(icon, states);
        
        int i=0;
        for(const auto& subMenu : subMenus)
        {
            i++;
            subMenu.draw(target, states);
        }
    }
    
    Menu& addSubmenu(std::string& iconPath)
    {
        subMenus.emplace_back(iconPath);
        updateSubmenu(subMenus[subMenus.size() - 1], subMenus.size());
        return subMenus.back();
    }
    
    void setFunction(std::function<void()>&& func)
    {
        actionFunc = func;
    }
    
    void clicked(const sf::Vector2f& point)
    {
        sf::Vector2f p = getInverseTransform().transformPoint(point);
        sf::FloatRect rect = icon.getLocalBounds();
        if (p.x>=0 && p.y>=0 && p.x < rect.width && p.y < rect.height)
        {
            std::cout << "Found button" << std::endl;
            if (actionFunc)
            {
                actionFunc();
            }
            else
            {
                m_isOpen = !m_isOpen;
            }
            
            return;
        }
        
        for(auto& subMenu : subMenus)
        {
            subMenu.clicked(p);
        }
    }
    
    void update()
    {
        float target = m_isOpen ? 1.0 : 0.0;
        if (target != m_transition)
        {
            //std::cout << "visible:" << m_isOpen << " transition: " << m_transition << " target: " << target << std::endl;
            m_transition += m_isOpen ? 0.05f : -0.05f;
            m_transition = std::min(std::max(m_transition, 0.0f), 1.0f);
            
            int i = 1;
            for(auto& subMenu : subMenus)
            {
                updateSubmenu(subMenu, i++);
            }
        }
    }
    
private:
    void updateSubmenu(Menu& subMenu, int i)
    {
        subMenu.setAlpha(m_transition);
        const float offset = -50 * i * m_transition;
        subMenu.setPosition(sf::Vector2f(offset, 0.0f));
    }
    
    void setAlpha(float m_transition)
    {
        icon.setColor(sf::Color(255,255,255, 255*m_transition));
    }
    
    sf::Texture texture; //TODO: perhaps get all the textures into one and just sprite them differently? should be more efficient, but do I care?
    sf::Sprite icon;
    std::vector<Menu> subMenus;
    std::function<void()> actionFunc;
    
    bool m_isOpen; // this describes the end state we're going for, the transition is controlled by m_transition
    float m_transition;
    
};

#endif