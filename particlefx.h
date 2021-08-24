#pragma once
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

class particlefx
	: public sf::Drawable, public sf::Transformable
{
    struct particle
    {
        sf::Vector2f velocity;
        sf::Time lifetime;
    };

    std::vector<particle> _particles;
    sf::VertexArray _vertices;

    sf::Vector2f _direction;
    int _scatter;
    sf::Time _lifetime;
    sf::Vector2f _emitter;

    sf::Color _color;
    std::function<sf::Color(float)> _colorfunc;

    void reset(uint32_t index);
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

public:
    particlefx(uint32_t count = 1000, const sf::Time& lifetime = sf::seconds(3.f));

    void set_count(uint32_t count);
    void set_direction(const sf::Vector2f& dir);
    void set_scatter(int degrees);
    void set_lifetime(const sf::Time& lifetime);
    void set_emitter(const sf::Vector2f& pos);
    void set_color(const sf::Color& color);
    void set_colorfunc(const std::function<sf::Color(float)>& colorfunc);

    void update(const sf::Time& elapsed);
};

