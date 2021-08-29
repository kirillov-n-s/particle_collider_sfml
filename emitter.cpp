#include "emitter.h"

particle* emitter::emit(const vec2f& coords)
{
	auto v = vec2f(_v(_engine), _v(_engine));
	float r, m, q;
	if (_random)
	{
		r = _r(_engine);
		m = r * MASS_UNIT;
		q = _q(_engine);
	}
	else
	{
		r = _sample_mass / MASS_UNIT;
		m = _sample_mass;
		q = _sample_charge;
	}
	return new particle(coords, v, r, m, q);
}

//public interface
emitter::emitter(uint32_t width, uint32_t height)
	: _width(width),
	_height(height),

	_engine(std::random_device()()),

	_x(MAX_RADIUS, width - MAX_RADIUS),
	_y(MAX_RADIUS, height - MAX_RADIUS),
	_v(-MAX_SPEED, MAX_SPEED),
	_r(MIN_RADIUS, MAX_RADIUS),
	_q(-MAX_CHARGE, MAX_CHARGE),

	_pos(0.f, 0.f),
	_max_pos(trunc(width / STEP) - 1.f, trunc(height / STEP) - 1.f)
{}

void emitter::reset()
{
	_pos = ZERO2;
	_max_pos = vec2f(trunc(_width / STEP) - 1.f, trunc(_height / STEP) - 1.f);
}

particle* emitter::operator()()
{
	vec2f p;
	if (!_uniform)
		p = vec2f(_x(_engine), _y(_engine));
	else
	{
		if (_pos.x > +_max_pos.x)
			if (_pos.y < +_max_pos.y)
				_pos = vec2f(0.f, _pos.y + 1.f);
			else
				throw std::exception("Particle Overflow!");
		p = _pos * STEP + vec2f(OFFSET, OFFSET);
		_pos += vec2f(1.f, 0.f);
	}
	return emit(p);
}

particle* emitter::operator()(const vec2f& coords)
{
	return emit(coords);
}

particle* emitter::operator()(const vec2f& coords, float mass, float charge)
{
	return new particle(coords, vec2f(_v(_engine), _v(_engine)), mass / MASS_UNIT, mass, charge);
}

//placement
bool emitter::is_uniform() const
{
	return _uniform;
}

void emitter::toggle_uniform()
{
	_uniform ^= true;
}

//mode
bool emitter::is_random() const
{
	return _random;
}

void emitter::toggle_random()
{
	_random ^= true;
	_sample_mass = MIN_RADIUS * MASS_UNIT;
	_sample_charge = 0.f;
}

//sample data & manip
float emitter::get_mass() const
{
	return _sample_mass;
}

float emitter::get_charge() const
{
	return _sample_charge;
}

void emitter::mod_mass(float value)
{
	_sample_mass += value;
	if (_sample_mass < MIN_RADIUS * MASS_UNIT)
		_sample_mass = MIN_RADIUS * MASS_UNIT;
}

void emitter::mod_charge(float value)
{
	_sample_charge += value;
}

void emitter::min_mass()
{
	_sample_mass = MIN_RADIUS * MASS_UNIT;
}

void emitter::null_charge()
{
	_sample_charge = 0.f;
}
