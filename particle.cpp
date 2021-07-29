#include "particle.h"

//general
void particle::move(const vec2& vector)
{
	_center += vector;
}

bool particle::collides(const vec2& point) const
{
	return ::distance(_center, point) <= _radius;
}

void particle::bounce(const vec2& vector)
{
	_velocity *= vector;
}

//particle-specific
float particle::distance(const particle& particle) const
{
	return ::distance(_center, particle._center);
}

vec2 particle::path(const particle& particle) const
{
	return ::path(_center, particle._center);
}

bool particle::collides(const particle& particle) const
{
	return distance(particle) < _radius + particle._radius;
}

bool particle::intersects_x(const particle& particle) const
{
	auto a1 = _center.x - _radius, b1 = _center.x + _radius;
	auto a2 = particle._center.x - particle._radius, b2 = particle._center.x + particle._radius;
	return a2 >= a1 && a2 <= b1 || b2 >= a1 && b2 <= b1
		|| a1 >= a2 && a1 <= b2 || b1 >= a2 && b1 <= b2;
}

//state manipulation
void particle::advance()
{
	_center += _velocity / (float)CYCLE_COUNT;
}

void particle::accelerate(const vec2& acceleration)
{
	_velocity += acceleration;// / (float)CYCLE_COUNT;// * (float)PERIOD;
}

//collision resolution
void particle::resolve_static(particle& particle)
{
	vec2 n = normalize(path(particle));
	float d = 0.5f * (distance(particle) - _radius - particle._radius);
	vec2 v = n * d;
	move(v);
	particle.move(-v);
}

void particle::resolve_dynamic(particle& particle)
{
	float m1 = _mass, m2 = particle._mass, msum = m1 + m2;
	vec2 v1 = _velocity, v2 = particle._velocity;
	float q1 = _charge, q2 = particle._charge;

	_velocity = (m1 - m2) / msum * v1 + 2.f * m2 / msum * v2;
	particle._velocity = 2.f * m1 / msum * v1 + (m2 - m1) / msum * v2;
	_charge = particle._charge = (q1 + q2) / 2.f;
}
