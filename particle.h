#pragma once
#include "vecops.h"

class particle
{
	vec2f _center;
	vec2f _velocity;
	float _radius;
	float _mass;
	float _charge;

public:
	particle(const vec2f& center, const vec2f& velocity, float radius, float mass, float charge);

	vec2f get_pos() const;
	vec2f get_vel() const;
	float get_rad() const;
	float get_mass() const;
	float get_charge() const;

	void move(const vec2f& vector);
	bool contains(const vec2f& point) const;
	void bounce(const vec2f& vector);

	void advance(float scalar = 1.f);
	void accelerate(const vec2f& acceleration = vec2f(0.f, 0.f), float scalar = 1.f);

	friend float dist(const particle& p1, const particle& p2);
	friend vec2f path(const particle& from, const particle& to);
	friend bool collides(const particle& p1, const particle& p2);
	friend bool intersects_x(const particle& p1, const particle& p2);
	friend bool intersects_y(const particle& p1, const particle& p2);

	friend void resolve_static(particle& p1, particle& p2);
	friend void resolve_dynamic(particle& p1, particle& p2);
};
