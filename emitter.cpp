#include "emitter.h"

//public interface
emitter::emitter(uint32_t width, uint32_t height, const vec2f& start)
	: _width(width),
	_height(height),
	_start(start),
	_pos(start),
	_max_pos(start + vec2f(trunc(width / STEP) - 1.f, trunc(height / STEP) - 1.f)),
	_engine(std::random_device()()),
	_v(-MAX_VELOCITY, MAX_VELOCITY),
	_r(MIN_RADIUS, MID_RADIUS),
	_q(-MAX_CHARGE, MAX_CHARGE)
{}

void emitter::reset()
{
	_pos = _start;
	_max_pos = _start + vec2f(trunc(_width / STEP) - 1.f, trunc(_height / STEP) - 1.f);
}

particle* emitter::operator()()
{
	if (_pos.x > _start.x + _max_pos.x)
		if (_pos.y < _start.y + _max_pos.y)
			_pos = vec2f(_start.x, _pos.y + 1.f);
		else
			throw std::exception("Particle Overflow!");
	auto c = _pos * STEP + vec2f(OFFSET, OFFSET);
	_pos += vec2f(1.f, 0.f);

	auto v = vec2f(_v(_engine), _v(_engine));
	float r, m, q;
	if (_random)
	{
		r = _r(_engine);
		m = r * MASS_SCALAR;
		q = _q(_engine);
	}
	else
	{
		r = _sample_mass / MASS_SCALAR;
		m = _sample_mass;
		q = _sample_charge;
	}
	return new particle(c, v, r, m, q);
}

particle* emitter::operator()(const vec2f& coords)
{
	auto v = vec2f(_v(_engine), _v(_engine));
	float r, m, q;
	if (_random)
	{
		r = _r(_engine);
		m = r * MASS_SCALAR;
		q = _q(_engine);
	}
	else
	{
		r = _sample_mass / MASS_SCALAR;
		m = _sample_mass;
		q = _sample_charge;
	}
	return new particle(coords, v, r, m, q);
}

particle* emitter::operator()(const vec2f& coords, float mass, float charge)
{
	auto v = vec2f(_v(_engine), _v(_engine));
	return new particle(coords, v, mass / MASS_SCALAR, mass, charge);
}

//sample data
bool emitter::is_random() const
{
	return _random;
}

float emitter::get_sample_mass() const
{
	return _sample_mass;
}

float emitter::get_sample_charge() const
{
	return _sample_charge;
}

//sample manip
void emitter::toggle_random()
{
	_random ^= true;
}

void emitter::adjust_mass(float value)
{
	_sample_mass += value;
	if (_sample_mass > MAX_RADIUS * MASS_SCALAR)
		_sample_mass = MAX_RADIUS * MASS_SCALAR;
	if (_sample_mass < MIN_RADIUS * MASS_SCALAR)
		_sample_mass = MIN_RADIUS * MASS_SCALAR;
}

void emitter::adjust_charge(float value)
{
	_sample_charge += value;
}

void emitter::average_mass()
{
	_sample_mass = MID_RADIUS * MASS_SCALAR;
}

void emitter::nullify_charge()
{
	_sample_charge = 0.f;
}

//sample display
particle* emitter::construct_sample(const vec2f& coords)
{
	float mass = _sample_mass;
	return new particle(coords, vec2f(MAX_VELOCITY, MAX_VELOCITY), mass / MASS_SCALAR, mass, _sample_charge);
}
