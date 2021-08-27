#pragma once
#include <functional>
#include <sstream>
#include <iomanip>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "collider.h"

class application
{
	collider* _collider;
	emitter* _emitter;

	std::string _title;
	sf::RenderWindow* _window;

	bool _paused = true;
	bool _graphic = true;

	sf::Font _font;
	const uint32_t _font_size = 32u;
	const vec2f _initial_offset = vec2f(8.f, 4.f);
	const vec2f _offset = vec2f(0.f, _font_size + _initial_offset.y);
	sf::Text _caption;
	void print(const std::string& string, const vec2f& pos);
	std::string nl(uint32_t n = 1);

	sf::RectangleShape _info;
	const uint32_t _info_width = 256;

	sf::RectangleShape _sample;
	const uint32_t _sample_width = _info_width - _initial_offset.x * 2.f;
	const uint32_t _sample_height = _initial_offset.y + _offset.y * 7.f + MAX_RADIUS * 2.f;
	vec2f _sample_offset;

	sf::Color colorize(float prim, float sec);
	sf::Drawable* picturize(particle* particle);
	sf::Drawable* symbolize(particle* particle);
	std::function<sf::Drawable* (particle*)> get_conversion();
	std::vector<sf::Drawable*> get_picture();

	bool _held = false;
	vec2f _pos;

	void handle_events();
	void update();
	void render();

	sf::Time _elapsed;

public:
	application(collider* collider, emitter* emitter, const std::string& title = "");
	~application();

	void load(uint32_t count = 0);
	void run();
};
