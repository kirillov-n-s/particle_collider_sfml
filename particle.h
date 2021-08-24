#pragma once
#include "geometry.h"
#include "const.h"

class particle
{
	vec2f _center;
	vec2f _velocity;
	float _radius;
	float _mass;
	float _charge;

public:
	particle(const vec2f& center, const vec2f& velocity, float radius, float mass, float charge)
		: _center(center), _velocity(velocity), _radius(radius), _mass(mass), _charge(charge) {}

	vec2f get_pos() const;
	vec2f get_vel() const;
	float get_rad() const;
	float get_mass() const;
	float get_charge() const;

	void move(const vec2f& vector);
	bool contains(const vec2f& point) const;
	void bounce(const vec2f& vector);

	float dist(const particle& particle) const;
	vec2f path(const particle& particle) const;
	bool collides(const particle& particle) const;
	bool intersects_x(const particle& particle) const;
	bool intersects_y(const particle& particle) const;

	void advance(float scalar = 1.f);
	void accelerate(const vec2f& acceleration = vec2f(0.f, 0.f), float scalar = 1.f);

	void resolve_static(particle& particle);
	void resolve_dynamic(particle& particle);
};
