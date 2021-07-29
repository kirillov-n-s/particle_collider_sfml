#pragma once
#include "geometry.h"
#include "utility.h"

class particle
{
	friend class collider;

	vec2 _center;
	vec2 _velocity;
	float _radius;
	float _mass;
	float _charge;

	void move(const vec2& vector);
	bool collides(const vec2& point) const;
	void bounce(const vec2& vector);

public:
	particle(const vec2& center, const vec2& velocity, float radius, float mass, float charge)
		: _center(center), _velocity(velocity), _radius(radius), _mass(mass), _charge(charge) {}

	float distance(const particle& particle) const;
	vec2 path(const particle& particle) const;
	bool collides(const particle& particle) const;
	bool intersects_x(const particle& particle) const;

	void advance();
	void accelerate(const vec2& acceleration = vec2(0.f, 0.f));

	void resolve_static(particle& particle);
	void resolve_dynamic(particle& particle);
};
