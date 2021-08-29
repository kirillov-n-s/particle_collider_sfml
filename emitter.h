#pragma once
#include <random>
#include "particle.h"

class emitter
{
	//particle limits
	const float MAX_SPEED = 2.f;
	const float MAX_CHARGE = 64.f;

	const float MIN_RADIUS = 4.f;
	const float MAX_RADIUS = 16.f;

	const float MASS_UNIT = 1.f;

	//positioning
	const float OFFSET = MAX_RADIUS + 2.f;
	const float STEP = 2.f * OFFSET;

	uint32_t _width;
	uint32_t _height;

	std::mt19937 _engine;
	std::uniform_real_distribution<float> _x;
	std::uniform_real_distribution<float> _y;
	std::uniform_real_distribution<float> _v;
	std::uniform_real_distribution<float> _r;
	std::uniform_real_distribution<float> _q;

	bool _uniform = true;
	vec2f _pos;
	vec2f _max_pos;

	bool _random = true;
	float _sample_mass = MIN_RADIUS * MASS_UNIT;
	float _sample_charge = 0.f;

	particle* emit(const vec2f& coords);

public:
	emitter(uint32_t width, uint32_t height);
	void reset();

	particle* operator()();
	particle* operator()(const vec2f& coords);
	particle* operator()(const vec2f& coords, float mass, float charge);

	bool is_uniform() const;
	void toggle_uniform();

	bool is_random() const;
	void toggle_random();

	float get_mass() const;
	float get_charge() const;
	void mod_mass(float value);
	void mod_charge(float value);
	void min_mass();
	void null_charge();
};

