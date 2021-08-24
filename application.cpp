#include "application.h"

//print text
void application::print(const std::string& string, const vec2f& start, uint32_t index)
{
	_caption.setString(string);
	_caption.setPosition(start + 2.f * index * _offset);
	_window->draw(_caption);
}

//drawing utils
sf::Color application::colorize(float prim, float sec)
{
	if (prim < 0.f)
	{
		if (sec > 127)
			return sf::Color((sec - 128) * 2, 255, 255);
		return sf::Color(0, sec * 2, 255);
	}

	if (sec > 127)
		return sf::Color(255, (sec - 128) * 2, (255 - sec) * 2);
	return sf::Color(255, 0, sec * 2);
}

sf::Drawable* application::picturize(particle* particle)
{
	float rad = particle->get_rad();
	vec2f pos = particle->get_pos();
	float e_param = _collider->get_elec_stat(particle);
	float m_param = _collider->get_mech_stat(particle) * 255.f;

	sf::CircleShape* projection = new sf::CircleShape(rad);
	projection->setPosition(pos.x - rad, pos.y - rad);
	projection->setFillColor(colorize(e_param, m_param));

	return projection;
}

sf::Drawable* application::symbolize(particle* particle)
{
	float rad = particle->get_rad();
	vec2f pos = particle->get_pos();
	float e_param = _collider->get_elec_stat(particle);
	float m_param = _collider->get_mech_stat(particle) * 255.f;

	char c_param = _collider->get_mech_stat(particle) * 93.f + 33;
	sf::Text* symbol = new sf::Text(c_param, _font, 2.f * rad);
	symbol->setPosition(pos.x - rad, pos.y - rad);
	symbol->setFillColor(colorize(e_param, m_param));

	return symbol;
}

std::function<sf::Drawable* (particle*)> application::get_conversion()
{
	if (_graphic)
		return [this](particle* p) { return picturize(p); };
	return [this](particle* p) { return symbolize(p); };
}

std::vector<sf::Drawable*> application::get_picture()
{
	const auto& particles = _collider->get_particles();
	auto result = std::vector<sf::Drawable*>(particles.size());
	std::transform(particles.begin(), particles.end(), result.begin(), get_conversion());
	return result;
}

//main loop stages
void application::handle_events()
{
	sf::Event event;
	while (_window->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			_window->close();

		if (event.type == sf::Event::KeyReleased)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Space:
				_paused ^= true;
				break;
			case sf::Keyboard::RAlt:
				_graphic ^= true;
				break;
			case sf::Keyboard::Enter:
				_collider->toggle_gravity();
				break;
			case sf::Keyboard::RControl:
				_collider->toggle_electricity();
				break;
			case sf::Keyboard::RShift:
				_emitter->toggle_random();
				break;

			case sf::Keyboard::C:
				_collider->clear();
				_emitter->reset();
				break;
			case sf::Keyboard::R:
			{
				auto count = _collider->count();
				_collider->clear();
				_emitter->reset();
				load(count);
				break;
			}

			case sf::Keyboard::Escape:
				_window->close();
				break;
			}

			if (_emitter->is_random())
				continue;
			switch (event.key.code)
			{
			case sf::Keyboard::M:
				_emitter->average_mass();
				break;
			case sf::Keyboard::N:
				_emitter->nullify_charge();
				break;
			}

		}

		if (event.type == sf::Event::KeyPressed)
		{
			if (_emitter->is_random())
				continue;
			switch (event.key.code)
			{
			case sf::Keyboard::Right:
				_emitter->adjust_mass(4.f);
				break;
			case sf::Keyboard::Left:
				_emitter->adjust_mass(-4.f);
				break;
			case sf::Keyboard::Up:
				_emitter->adjust_charge(4.f);
				break;
			case sf::Keyboard::Down:
				_emitter->adjust_charge(-4.f);
				break;
			}
		}

		if (event.type == sf::Event::MouseButtonPressed)
		{
			auto mouse = vec2f(sf::Mouse::getPosition(*_window));
			if (mouse.x > _collider->width())
				continue;

			switch (event.mouseButton.button)
			{
			case sf::Mouse::Right:
				if (_held)
					break;
				_held = true;
				_forcepoint = mouse;
				break;
			}
		}

		if (event.type == sf::Event::MouseButtonReleased)
		{
			auto mouse = vec2f(sf::Mouse::getPosition(*_window));
			if (mouse.x > _collider->width())
				continue;

			particle* particle;
			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left:
				particle = _collider->get(mouse);
				if (particle)
					_collider->erase(particle);
				else
				{
					particle = (*_emitter)(mouse);
					_collider->launch(particle);
				}
				break;
			case sf::Mouse::Right:
				_held = false;
				_collider->apply_force(_forcepoint, mouse - _forcepoint);
				break;
			}
		}
	}
}

void application::update()
{
	_collider->operate();
}

void application::render()
{
	auto namegrav = [this]()
	{
		switch (_collider->get_gravity())
		{
		case 1:
			return "natural";
		case -1:
			return "inverted";
		default:
			return "none";
		}
	};

	_window->clear();

	const auto& drawables = get_picture();
	for (auto drawable : drawables)
	{
		_window->draw(*drawable);
		delete drawable;
	}

	_window->draw(_info);
	
	int i = -1;
	vec2f info_pos = _info.getPosition() + _initial_offset;
	print("count:\n\t" + std::to_string(_collider->count()), info_pos, ++i);
	print("gravity:\n\t" + std::string(namegrav()), info_pos, ++i);
	print("electricity:\n\t" + std::string(_collider->get_electricity() ? "on" : "off"), info_pos, ++i);
	print(_paused ? "PAUSED ||" : "PLAYING |>", info_pos, ++++i);

	_window->draw(_sample);

	i = 0;
	vec2f sample_pos = info_pos + _sample_offset + _initial_offset;
	bool rnd = _emitter->is_random();
	print("launch:\n\t" + std::string(rnd ? "RANDOM" : "CUSTOM"), sample_pos, i++);
	print("mass:\n\t" + (rnd ? "?" : std::to_string(_emitter->get_sample_mass())), sample_pos, i++);
	print("charge:\n\t" + (rnd ? "?" : std::to_string(_emitter->get_sample_charge())), sample_pos, i++);

	if (!_emitter->is_random())
	{
		vec2f offset = 2.f * 3 * _offset - _initial_offset;
		sample_pos = vec2f(info_pos.x + _sample_width / 2, sample_pos.y + _sample_height / 2 + offset.y / 2.f);
		auto sample = _emitter->construct_sample(sample_pos);
		auto drawable = get_conversion()(sample);
		_window->draw(*drawable);
		delete sample;
		delete drawable;
	}

	_window->display();
}

//public interface
application::application(collider* collider, emitter* emitter, const std::string& title)
	: _collider(collider), _emitter(emitter), _title(title)
{
	auto width = _collider->width();
	auto height = _collider->height();

	_window = new sf::RenderWindow(sf::VideoMode(width + _info_width, height), _title);
	_window->setVerticalSyncEnabled(true);

	_font.loadFromFile("cour.ttf");

	auto bg = sf::Color::Black;
	auto main = sf::Color::White;
	auto thickness = -4.f;

	_info = sf::RectangleShape(vec2f(_info_width, height));
	_info.setPosition(vec2f(width, 0.f));
	_info.setFillColor(bg);
	_info.setOutlineColor(main);
	_info.setOutlineThickness(thickness);

	_sample_offset = vec2f(_initial_offset.x * 2.f, height - _sample_height - _initial_offset.y * 2.f);
	_sample = sf::RectangleShape(vec2f(_sample_width, _sample_height));
	_sample.setPosition(vec2f(width + _initial_offset.x, _sample_offset.y));
	_sample.setFillColor(bg);
	_sample.setOutlineColor(main);
	_sample.setOutlineThickness(thickness);

	_caption = sf::Text("", _font, _font_size);
	_caption.setFillColor(main);
}

application::~application()
{
	delete _window;
}

void application::load(uint32_t count)
{
	for (int i = 0; i < count; i++)
	{
		auto particle = (*_emitter)();
		_collider->launch(particle);
	}
}

void application::run()
{
	sf::Clock clock;
	while (_window->isOpen())
	{
		handle_events();
		if (!_paused)
			update();
		render();

		_elapsed = clock.restart();
		auto fps = 1.f / _elapsed.asSeconds();
		_window->setTitle(_title + " [FPS: " + std::to_string((int)fps) + "]");
	}
}
