#include "application.h"

void application::fluctuate(sf::Color& color)
{
	int dr = _distrib(_engine);
	int dg = _distrib(_engine);
	int db = _distrib(_engine);

	int r = color.r + dr;
	int g = color.g + dg;
	int b = color.b + db;
	
	/*for (auto x : {&r, &g, &b})
	{
		if (*x < 0)
			*x = 0;
		else if (*x > 255)
			*x = 255;
	}*/

	color = sf::Color(r, g, b);
}

sf::Drawable* application::picturize(particle* particle)
{
	float rad = _collider->get_radius(particle);
	vec2 pos = _collider->get_position(particle);
	uint8_t m_param = _shaping ? _collider->get_mech_param(particle) * 12.f : 20;
	if (m_param < 3)
		m_param = 3;
	float e_param = _collider->get_elec_param(particle) * 127.f;
	if (e_param < 0.f)
		e_param = -e_param + 128.f;

	sf::CircleShape* projection = new sf::CircleShape(rad, m_param);
	projection->setPosition(pos.x - rad, pos.y - rad);

	/*if (m_param > 191)
		color = sf::Color((255 - m_param) * 4, 255, 255);
	else if (m_param > 127)
		color = sf::Color(255, 255, 255 - (m_param - 191) * 4);
	else if (m_param > 63)
		color = sf::Color(255, 255 + (m_param - 127) * 4, 0);
	else
		color = sf::Color(255, 0, -4 * (m_param - 63));
	projection->setOutlineColor(color);
	projection->setOutlineThickness(rad);*/
	
	/*if (e_param > 191)
		color = sf::Color(0, (255 - e_param) * 4, 255);
	else if (e_param > 127)
		color = sf::Color(-4 * (e_param - 191), 255, 255);
	else if (e_param > 63)
		color = sf::Color(255, -4 * (e_param - 127), 0);
	else
		color = sf::Color(255, 255, -4 * (e_param - 63));*/

	sf::Color color;
	if (e_param > 191)
		color = sf::Color(0, (255 - e_param) * 4, 255);
	else if (e_param > 127)
		color = sf::Color(0, 255, 4 * (e_param - 128));
	else if (e_param > 63)
		color = sf::Color(255, -4 * (e_param - 127), 0);
	else
		color = sf::Color(e_param * 4, 255, 0);
	if (_colorful)
		fluctuate(color);
	projection->setFillColor(color);

	return projection;
}

sf::Drawable* application::symbolize(particle* particle)
{
	float rad = _collider->get_radius(particle);
	vec2 pos = _collider->get_position(particle);
	uint8_t m_param = (uint8_t)(_collider->get_mech_param(particle) * 93.f) + 33;
	float e_param = _collider->get_elec_param(particle) * 127.f;
	if (e_param < 0.f)
		e_param = -e_param + 128.f;

	sf::Text* symbol = new sf::Text((char)m_param, _font, 2.f * rad);
	symbol->setPosition(pos.x - rad, pos.y - rad);

	sf::Color color;
	if (e_param > 191)
		color = sf::Color(0, (255 - e_param) * 4, 255);
	else if (e_param > 127)
		color = sf::Color(0, 255, 4 * (e_param - 128));
	else if (e_param > 63)
		color = sf::Color(255, -4 * (e_param - 127), 0);
	else
		color = sf::Color(e_param * 4, 255, 0);
	if (_colorful)
		fluctuate(color);
	symbol->setFillColor(color);

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
	const auto& particles = _collider->particles();
	auto result = std::vector<sf::Drawable*>(particles.size());
	std::transform(particles.begin(), particles.end(), result.begin(), get_conversion());
	return result;
}

std::string application::name_gravity()
{
	switch (_collider->gravity())
	{
	case 1:
		return "natural";
	case -1:
		return "inverted";
	default:
		return "none";
	}
}

void application::draw_next(const std::string& string, const vec2& start, uint32_t index)
{
	_caption.setString(string);
	_caption.setPosition(start + 2.f * index * _offset);
	_window->draw(_caption);
}

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
				_paused = !_paused;
				break;
			case sf::Keyboard::RAlt:
				_graphic = !_graphic;
				break;
			case sf::Keyboard::BackSpace:
				_colorful = !_colorful;
				break;
			case sf::Keyboard::BackSlash:
				_shaping = !_shaping;
				break;
			case sf::Keyboard::Enter:
				_collider->toggle_gravity();
				break;
			case sf::Keyboard::RControl:
				_collider->toggle_electricity();
				break;
			case sf::Keyboard::RShift:
				_collider->toggle_random();
				break;
			case sf::Keyboard::Escape:
				_window->close();
				break;
			}

			if (_collider->random())
				continue;
			switch (event.key.code)
			{
			case sf::Keyboard::Right:
				_collider->adjust_mass(4.f);
				break;
			case sf::Keyboard::Left:
				_collider->adjust_mass(-4.f);
				break;
			case sf::Keyboard::Up:
				_collider->adjust_charge(4.f);
				break;
			case sf::Keyboard::Down:
				_collider->adjust_charge(-4.f);
				break;
			case sf::Keyboard::Home:
				_collider->flip_mass();
				break;
			case sf::Keyboard::End:
				_collider->flip_charge();
				break;
			case sf::Keyboard::Insert:
				_collider->average_mass();
				break;
			case sf::Keyboard::Delete:
				_collider->nullify_charge();
				break;
			}

		}

		if (event.type == sf::Event::MouseButtonReleased)
		{
			auto pos = vec2(sf::Mouse::getPosition(*_window));
			if (pos.x > _collider->width())
				continue;

			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left:
				_collider->launch(pos);
				break;
			case sf::Mouse::Right:
				_collider->erase(pos);
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
	_window->clear(sf::Color::Black);

	const auto& drawables = get_picture();
	for (auto drawable : drawables)
	{
		_window->draw(*drawable);
		delete drawable;
	}

	_window->draw(_info);
	
	vec2 info_pos = _info.getPosition() + _initial_offset;
	draw_next("count:\n\t" + std::to_string(_collider->count()), info_pos);
	draw_next("gravity:\n\t" + name_gravity(), info_pos, 1);
	draw_next("electricity:\n\t" + std::string(_collider->electricity() ? "on" : "off"), info_pos, 2);
	draw_next(_paused ? "PAUSED ||" : "PLAYING |>", info_pos, 3);

	_window->draw(_sample);

	vec2 sample_pos = info_pos + _sample_offset + _initial_offset;
	bool rnd = _collider->random();
	draw_next("launch:\n\t" + std::string(rnd ? "RANDOM" : "CUSTOM"), sample_pos);
	draw_next("mass:\n\t" + (rnd ? "?" : std::to_string(_collider->sample_mass())), sample_pos, 1);
	draw_next("charge:\n\t" + (rnd ? "?" : std::to_string(_collider->sample_charge())), sample_pos, 2);

	if (!_collider->random())
	{
		vec2 offset = 2.f * 3 * _offset - _initial_offset;
		sample_pos = vec2(info_pos.x + _sample_width / 2, sample_pos.y + _sample_height / 2 + offset.y / 2.f);
		auto sample = _collider->construct_sample(sample_pos);
		auto drawable = get_conversion()(sample);
		_window->draw(*drawable);
		delete sample;
		delete drawable;
	}

	_window->display();
}

application::application(collider* collider, const std::string& title)
	: _collider(collider), _title(title)
{
	auto width = _collider->width();
	auto height = _collider->height();

	_window = new sf::RenderWindow(sf::VideoMode(width + _info_width, height), _title);
	_window->setVerticalSyncEnabled(true);

	_font.loadFromFile("cour.ttf");

	auto fill = sf::Color::Black;
	auto outline = sf::Color::Green;
	auto thickness = -4.f;

	_info = sf::RectangleShape(vec2(_info_width, height));
	_info.setPosition(vec2(width, 0.f));
	_info.setFillColor(fill);
	_info.setOutlineColor(outline);
	_info.setOutlineThickness(thickness);

	_sample_offset = vec2(_initial_offset.x * 2.f, height - _sample_height - _initial_offset.y * 2.f);
	_sample = sf::RectangleShape(vec2(_sample_width, _sample_height));
	_sample.setPosition(vec2(width + _initial_offset.x, _sample_offset.y));
	_sample.setFillColor(fill);
	_sample.setOutlineColor(outline);
	_sample.setOutlineThickness(thickness);

	_caption = sf::Text("", _font, _font_size);
	_caption.setFillColor(sf::Color::Green);
}

application::~application()
{
	delete _window;
}

void application::load(uint32_t count)
{
	for (int i = 0; i < count; i++)
		_collider->launch();
}

void application::run()
{
	while (_window->isOpen())
	{
		std::chrono::system_clock clock;
		auto then = clock.now();

		handle_events();
		if (!_paused)
			update();
		render();

		long double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - then).count();
		_window->setTitle(_title + " [FPS: " + std::to_string(1000.l / elapsed) + "]");
	}
}
