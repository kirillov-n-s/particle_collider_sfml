#pragma once

#include <random>
#include "particle.h"

class emitter
{
	const float OFFSET = MID_RADIUS + 2.f;
	const float STEP = 2.f * OFFSET;

	vec2 _pos;
	vec2 _max_pos;

	std::mt19937 _engine;
	std::uniform_real_distribution<float> _v;
	std::uniform_real_distribution<float> _r;
	std::uniform_real_distribution<float> _q;

public:
	emitter(uint32_t width, uint32_t height);

	particle* operator()();
	particle* operator()(const vec2& coords);
	particle* operator()(const vec2& coords, float mass, float charge);
};

