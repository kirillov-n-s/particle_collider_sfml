#pragma once
#include <vector>
#include <chrono>
#include <functional>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "collider.h"

class application
{
	std::string _title;
	sf::RenderWindow* _window;
	collider* _collider;

	bool _paused = true;
	bool _graphic = true;
	bool _shaping = false;
	bool _colorful = false;

	sf::Font _font;
	const uint32_t _font_size = 32u;
	const vec2 _initial_offset = vec2(8.f, 4.f);
	const vec2 _offset = vec2(0.f, _font_size + _initial_offset.y);
	sf::Text _caption;

	sf::RectangleShape _info;
	const uint32_t _info_width = 256u;

	sf::RectangleShape _sample;
	const uint32_t _sample_width = _info_width - _initial_offset.x * 2.f;
	const uint32_t _sample_height = _initial_offset.y + _offset.y * 7.f + MAX_RADIUS * 2.f;
	vec2 _sample_offset;

	std::random_device _device;
	std::minstd_rand _engine = std::minstd_rand(_device());
	std::uniform_int_distribution<int> _distrib = std::uniform_int_distribution<int>(-64, 64);
	void fluctuate(sf::Color& color);

	sf::Drawable* picturize(particle* particle);
	sf::Drawable* symbolize(particle* particle);
	std::function<sf::Drawable* (particle*)> get_conversion();
	std::vector<sf::Drawable*> get_picture();

	std::string name_gravity();
	void draw_next(const std::string& string, const vec2& start, uint32_t index = 0);

	void handle_events();
	void update();
	void render();

public:
	application(collider* collider, const std::string& title = "");
	~application();

	void load(uint32_t count = 0);
	void run();
};
