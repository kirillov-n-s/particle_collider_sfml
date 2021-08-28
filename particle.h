#pragma once
#include <vector>
#include "vecops.h"

class particle
{
	vec2f _center;
	vec2f _velocity = vec2f(0.f, 0.f);
	vec2f _acceleration = vec2f(0.f, 0.f);
	float _radius;
	float _mass;
	float _charge = 0.f;

public:
	/*struct props
	{
		vec2f pos;
		float rad;
		float mass;

		vec2f vel = { 0.f, 0.f };
		vec2f acc = { 0.f, 0.f };
		float charge = 0.f;
	};*/

	particle(const vec2f& pos, const vec2f& vel, float rad, float mass, float charge);

	vec2f pos() const;
	vec2f velocity() const;
	vec2f acceleration() const;
	float rad() const;
	float mass() const;
	float charge() const;

	void move(const vec2f& vector);
	bool contains(const vec2f& point) const;
	void bounce(const vec2f& vector);

	void advance(float scalar = 1.f);
	void apply_newton(const std::vector<particle*> particles, float scalar);
	void apply_coulomb(const std::vector<particle*> particles, float scalar);
	void apply_drag(float drag);
	void apply_force(const vec2f& force, const vec2f& point);

	friend float dist(const particle& p1, const particle& p2);
	friend vec2f path(const particle& from, const particle& to);
	friend bool collides(const particle& p1, const particle& p2);
	friend bool intersects_x(const particle& p1, const particle& p2);
	friend bool intersects_y(const particle& p1, const particle& p2);

	friend void resolve_static(particle& p1, particle& p2);
	friend void resolve_dynamic(particle& p1, particle& p2);
};
