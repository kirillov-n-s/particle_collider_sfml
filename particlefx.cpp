#include "particlefx.h"

void particlefx::reset(uint32_t index)
{
    const float PI = 3.14159265f;
    const float ALPHA = PI / 180.f;

    float angle;
    float speed = speed = std::rand() % 50 + 50.f;;
    if (_direction == sf::Vector2f(0.f, 0.f))
        angle = std::rand() % 360 * ALPHA;
    else
    {
        float randangle = _scatter ? (std::rand() % 2 ? 1 : -1) * std::rand() % _scatter * ALPHA : 0.f;
        angle = std::atan2(_direction.y, _direction.x) + randangle;
    }

    _particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
    _particles[index].lifetime = sf::milliseconds(std::rand() % _lifetime.asMilliseconds());
    _vertices[index].position = _emitter;
}

void particlefx::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = nullptr;
    target.draw(_vertices, states);
}

//public interface
particlefx::particlefx(uint32_t count, const sf::Time& lifetime)
    : _particles(count),
    _vertices(sf::Points, count),
    _direction(0.f, 0.f),
    _scatter(180),
    _lifetime(lifetime),
    _emitter(0.f, 0.f),
    _color(sf::Color::White),
    _colorfunc([this](float ratio) { return sf::Color(_color.r, _color.g, _color.b, ratio * 255); })
{}

//setters
void particlefx::set_count(uint32_t count)
{
    _particles.resize(count);
    _vertices.resize(count);
}

void particlefx::set_direction(const sf::Vector2f& dir)
{
    _direction = dir;
}

void particlefx::set_scatter(int degrees)
{
    _scatter = degrees;
}

void particlefx::set_lifetime(const sf::Time& lifetime)
{
    _lifetime = lifetime;
}

void particlefx::set_emitter(const sf::Vector2f& pos)
{
    _emitter = pos;
}

void particlefx::set_color(const sf::Color& color)
{
    _color = color;
    for (int i = 0; i < _particles.size(); i++)
        _vertices[i].color = _color;
}

void particlefx::set_colorfunc(const std::function<sf::Color(float)>& colorfunc)
{
    _colorfunc = colorfunc;
}

//call each frame
void particlefx::update(const sf::Time& elapsed)
{
    for (std::size_t i = 0; i < _particles.size(); ++i)
    {
        particle& p = _particles[i];
        p.lifetime -= elapsed;
        if (p.lifetime <= sf::Time::Zero)
            reset(i);

        _vertices[i].position += p.velocity * elapsed.asSeconds();
        _vertices[i].color = _colorfunc(p.lifetime / _lifetime);
    }
}
