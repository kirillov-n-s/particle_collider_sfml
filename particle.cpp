#include "particle.h"

//getters
vec2f particle::get_pos() const
{
	return _center;
}

vec2f particle::get_vel() const
{
	return _velocity;
}

float particle::get_rad() const
{
	return _radius;
}

float particle::get_mass() const
{
	return _mass;
}

float particle::get_charge() const
{
	return _charge;
}

//general ops
void particle::move(const vec2f& vector)
{
	_center += vector;
}

bool particle::contains(const vec2f& point) const
{
	return ::dist(_center, point) <= _radius;
}

void particle::bounce(const vec2f& vector)
{
	_velocity *= vector;
}

//particle-specific ops
float particle::dist(const particle& particle) const
{
	return ::dist(_center, particle._center);
}

vec2f particle::path(const particle& particle) const
{
	return particle._center - _center;
}

bool particle::collides(const particle& particle) const
{
	return dist(particle) < _radius + particle._radius;
}

bool particle::intersects_x(const particle& particle) const
{
	auto a1 = _center.x - _radius;
	auto b1 = _center.x + _radius;
	auto a2 = particle._center.x - particle._radius;
	auto b2 = particle._center.x + particle._radius;

	return a2 >= a1 && a2 <= b1 || b2 >= a1 && b2 <= b1
		|| a1 >= a2 && a1 <= b2 || b1 >= a2 && b1 <= b2;
}

bool particle::intersects_y(const particle& particle) const
{
	auto a1 = _center.y - _radius;
	auto b1 = _center.y + _radius;
	auto a2 = particle._center.y - particle._radius;
	auto b2 = particle._center.y + particle._radius;

	return a2 >= a1 && a2 <= b1 || b2 >= a1 && b2 <= b1
		|| a1 >= a2 && a1 <= b2 || b1 >= a2 && b1 <= b2;
}

//state manipulation
void particle::advance(float scalar)
{
	_center += _velocity * scalar;
}

void particle::accelerate(const vec2f& acceleration, float scalar)
{
	_velocity += acceleration * scalar;
}

//collision resolution
void particle::resolve_static(particle& particle)
{
	vec2f n = norm(path(particle));
	float d = 0.5f * (dist(particle) - _radius - particle._radius);
	vec2f v = n * d;
	move(v);
	particle.move(-v);
}

void particle::resolve_dynamic(particle& particle)
{
	float m1 = _mass, m2 = particle._mass, msum = m1 + m2;
	vec2f v1 = _velocity, v2 = particle._velocity;
	float q1 = _charge, q2 = particle._charge;

	_velocity = (m1 - m2) / msum * v1 + 2.f * m2 / msum * v2;
	particle._velocity = 2.f * m1 / msum * v1 + (m2 - m1) / msum * v2;
	_charge = particle._charge = (q1 + q2) / 2.f;
}
