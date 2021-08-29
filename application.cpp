#include "application.h"

//text utils
void application::print(const std::string& string, const vec2f& pos)
{
	_text.setString(string);
	_text.setPosition(pos);
	_window->draw(_text);
}

std::string application::nl(uint32_t n)
{
	std::string res;
	for (int i = 0; i < n; i++)
		res += '\n';
	return res;
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
	float rad = particle->rad();
	vec2f pos = particle->pos();
	float e_param = _collider->elec_stat(particle);
	float m_param = _collider->mech_stat(particle) * 255.f;

	sf::CircleShape* projection = new sf::CircleShape(rad);
	projection->setPosition(pos.x - rad, pos.y - rad);
	projection->setFillColor(colorize(e_param, m_param));

	return projection;
}

sf::Drawable* application::symbolize(particle* particle)
{
	float rad = particle->rad();
	vec2f pos = particle->pos();
	float e_param = _collider->elec_stat(particle);
	float m_param = _collider->mech_stat(particle) * 255.f;

	char c_param = _collider->mech_stat(particle) * 93.f + 33;
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

std::vector<sf::Drawable*> application::get_drawables()
{
	const auto& particles = _collider->particles();
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
			case sf::Keyboard::RControl:
				_emitter->toggle_random();
				break;
			case sf::Keyboard::RShift:
				_emitter->toggle_uniform();
				break;

			case sf::Keyboard::F:
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

			case sf::Keyboard::Z:
				_collider->toggle_newton();
				break;
			case sf::Keyboard::X:
				_collider->toggle_coulomb();
				break;
			case sf::Keyboard::C:
				_collider->toggle_drag();
				break;

			case sf::Keyboard::T:
				_collider->set_E_field(ZERO2);
				break;
			case sf::Keyboard::G:
				_collider->set_G_field(ZERO2);
				break;
			case sf::Keyboard::B:
				_collider->set_B_field(ZERO3);
				break;

			case sf::Keyboard::Y:
				_collider->set_E_field(UP);
				break;
			case sf::Keyboard::U:
				_collider->set_E_field(DOWN);
				break;
			case sf::Keyboard::I:
				_collider->set_E_field(LEFT);
				break;
			case sf::Keyboard::O:
				_collider->set_E_field(RIGHT);
				break;

			case sf::Keyboard::H:
				_collider->set_G_field(UP);
				break;
			case sf::Keyboard::J:
				_collider->set_G_field(DOWN);
				break;
			case sf::Keyboard::K:
				_collider->set_G_field(LEFT);
				break;
			case sf::Keyboard::L:
				_collider->set_G_field(RIGHT);
				break;

			case sf::Keyboard::N:
				_collider->set_B_field(FORWARD);
				break;
			case sf::Keyboard::M:
				_collider->set_B_field(BACKWARD);
				break;


			case sf::Keyboard::Escape:
				_window->close();
				break;
			}

			if (_emitter->is_random())
				continue;
			switch (event.key.code)
			{
			case sf::Keyboard::Delete:
				_emitter->null_charge();
				break;
			case sf::Keyboard::Home:
				_emitter->min_mass();
				break;
			}

		}

		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::D:

				break;
			case sf::Keyboard::A:

				break;
			case sf::Keyboard::W:

				break;
			case sf::Keyboard::S:

				break;
			case sf::Keyboard::E:

				break;
			case sf::Keyboard::Q:

				break;
			}

			if (_emitter->is_random())
				continue;
			switch (event.key.code)
			{
			case sf::Keyboard::Right:
				_emitter->mod_mass(1.f);
				break;
			case sf::Keyboard::Left:
				_emitter->mod_mass(-1.f);
				break;
			case sf::Keyboard::Up:
				_emitter->mod_charge(1.f);
				break;
			case sf::Keyboard::Down:
				_emitter->mod_charge(-1.f);
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
			case sf::Mouse::Left:
				if (_held)
					break;
				_held = true;
				_pos = mouse;
				break;
			}
		}

		if (event.type == sf::Event::MouseButtonReleased)
		{
			auto mouse = vec2f(sf::Mouse::getPosition(*_window));
			if (mouse.x > _collider->width())
				continue;

			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left:
				_held = false;
				_collider->apply_force(_pos, mouse - _pos);
				break;
			}
		}

		if (event.type == sf::Event::MouseWheelScrolled)
		{
			auto mouse = vec2f(sf::Mouse::getPosition(*_window));
			if (mouse.x > _collider->width())
				continue;

			if (event.mouseWheelScroll.delta > 0.f)
				_collider->launch((*_emitter)(mouse));
			else
				_collider->erase();
		}
	}
}

void application::update()
{
	_collider->operate();
}

void application::render()
{
	auto gfield = _collider->get_G_field();
	auto emfield = _collider->get_EM_field();
	std::string newton;
	switch (_collider->get_newton())
	{
	case 1:
		newton = "natural";
		break;
	case -1:
		newton = "inverted";
		break;
	default:
		newton = "off";
		break;
	}

	_window->clear();

	const auto& drawables = get_drawables();
	for (auto drawable : drawables)
	{
		_window->draw(*drawable);
		delete drawable;
	}

	_window->draw(_info);
	vec2f ipos = _info.getPosition() + _start;
	std::stringstream str;
	str << std::fixed << std::setprecision(0)
		<< "count:\n\t" << _collider->count() << nl(2)
		<< "newton's law:\n\t" << newton << nl()
		<< "coulomb's law:\n\t" << (_collider->get_coulomb() ? "on" : "off") << nl()
		<< "drag:\n\t" << (_collider->get_drag() ? "on" : "off") << nl(2)
		<< "gravity field:\n\t(" << gfield.x << ", " << gfield.y << ")" << nl()
		<< "EM field:\n\t(" << emfield.x << ", " << emfield.y << ", " << emfield.z << ")" << nl(2)
		<< (_paused ? "PAUSED ||" : "PLAYING |>");
	print(str.str(), ipos);

	_window->draw(_props);
	vec2f ppos = ipos + _poffset + _start;
	str = std::stringstream();
	str << "placement:\n\t"
		<< (_emitter->is_uniform() ? "UNIFORM" : "RANDOM") << nl()
		<< "props:\n\t";
	if (_emitter->is_random())
		str << "RANDOM";
	else
		str << std::fixed << std::setprecision(1)
			<< "CUSTOM" << nl(2)
			<< "mass:\n\t" << _emitter->get_mass() << nl()
			<< "charge:\n\t" << _emitter->get_charge();
	print(str.str(), ppos);

	_window->display();
}

//public interface
application::application(collider* collider, emitter* emitter, const std::string& title)
	: _collider(collider), _emitter(emitter), _title(title)
{
	auto width = _collider->width();
	auto height = _collider->height();

	_window = new sf::RenderWindow(sf::VideoMode(width + _iwidth, height), _title);
	_window->setVerticalSyncEnabled(true);

	auto bg = sf::Color::Black;
	auto main = sf::Color::White;
	auto thickness = -4.f;

	_info = sf::RectangleShape(vec2f(_iwidth, height));
	_info.setPosition(vec2f(width, 0.f));
	_info.setFillColor(bg);
	_info.setOutlineColor(main);
	_info.setOutlineThickness(thickness);

	_poffset = vec2f(_start.x, height - _psize.y - _start.y * 2.f);
	_props = sf::RectangleShape(_psize);
	_props.setPosition(vec2f(width + _start.x, _poffset.y));
	_props.setFillColor(bg);
	_props.setOutlineColor(main);
	_props.setOutlineThickness(thickness);

	_font.loadFromFile("cour.ttf");
	_text = sf::Text("", _font, _fsize);
	_text.setFillColor(main);
}

application::~application()
{
	delete _window;
}

void application::load(uint32_t count)
{
	for (int i = 0; i < count; i++)
		_collider->launch((*_emitter)());
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
