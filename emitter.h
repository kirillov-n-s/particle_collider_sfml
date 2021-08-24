#pragma once
#include <random>
#include "particle.h"
#include "const.h"

class emitter
{
	const float OFFSET = MID_RADIUS + 2.f;
	const float STEP = 2.f * OFFSET;

	uint32_t _width;
	uint32_t _height;

	vec2f _start;
	vec2f _pos;
	vec2f _max_pos;

	std::mt19937 _engine;
	std::uniform_real_distribution<float> _v;
	std::uniform_real_distribution<float> _r;
	std::uniform_real_distribution<float> _q;

	bool _random = true;
	float _sample_mass = MIN_RADIUS * MASS_SCALAR;
	float _sample_charge = 0.f;

public:
	emitter(uint32_t width, uint32_t height, const vec2f& start = vec2f(0.f, 0.f));
	void reset();

	particle* operator()();
	particle* operator()(const vec2f& coords);
	particle* operator()(const vec2f& coords, float mass, float charge);

	bool is_random() const;
	float get_sample_mass() const;
	float get_sample_charge() const;

	void toggle_random();
	void adjust_mass(float value);
	void adjust_charge(float value);
	void average_mass();
	void nullify_charge();

	particle* construct_sample(const vec2f& coords);
};

