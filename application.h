#pragma once
#include <functional>
#include <sstream>
#include <iomanip>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "collider.h"
#include "emitter.h"

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
	const vec2f _start = vec2f(8.f, 4.f);
	const vec2f _offset = vec2f(0.f, _font_size + _start.y);
	sf::Text _text;
	void print(const std::string& string, const vec2f& pos);
	std::string nl(uint32_t n = 1);

	sf::RectangleShape _info;
	const uint32_t _info_width = 256;

	sf::RectangleShape _emitprops;
	const uint32_t _emitprops_width = _info_width - _start.x * 2.f;
	const uint32_t _emitprops_height = _start.y + _offset.y * 9.5f;
	vec2f _emitprops_offset;

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
