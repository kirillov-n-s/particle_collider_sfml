#include "emitter.h"

emitter::emitter(uint32_t width, uint32_t height)
	: _pos(vec2(0.f, 0.f)), _max_pos(vec2(truncf(width / STEP) - 1.f, truncf(height / STEP) - 1.f))
{
	std::random_device device;
	_engine = std::mt19937(device());
	_v = std::uniform_real_distribution<float>(-MAX_VELOCITY, MAX_VELOCITY);
	_r = std::uniform_real_distribution<float>(MIN_RADIUS, MID_RADIUS);
	_q = std::uniform_real_distribution<float>(-MAX_CHARGE, MAX_CHARGE);
}

particle* emitter::operator()()
{
	if (_pos.x > _max_pos.x)
		if (_pos.y < _max_pos.y)
			_pos = vec2(0.f, _pos.y + 1.f);
		else
			throw std::exception("Particle Overflow!");
	auto c = _pos * STEP + vec2(OFFSET, OFFSET);
	_pos += vec2(1.f, 0.f);

	auto v = vec2(_v(_engine), _v(_engine));
	auto r = _r(_engine);
	auto m = r * MASS_SCALAR;
	auto q = _q(_engine);
	return new particle(c, v, r, m, q);
}

particle* emitter::operator()(const vec2& coords)
{
	auto v = vec2(_v(_engine), _v(_engine));
	auto r = _r(_engine);
	auto m = r * MASS_SCALAR;
	auto q = _q(_engine);
	return new particle(coords, v, r, m, q);
}

particle* emitter::operator()(const vec2& coords, float mass, float charge)
{
	auto v = vec2(_v(_engine), _v(_engine));
	return new particle(coords, v, mass / MASS_SCALAR, mass, charge);
}
