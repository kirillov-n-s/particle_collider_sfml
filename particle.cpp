#include "particle.h"

particle::particle(const vec2f& pos, const vec2f& vel, float rad, float mass, float charge)
	: _center(pos),
	_velocity(vel),
	_radius(rad),
	_mass(mass),
	_charge(charge)
{}

//getters
vec2f particle::pos() const
{
	return _center;
}

vec2f particle::velocity() const
{
	return _velocity;
}

vec2f particle::acceleration() const
{
	return _acceleration;
}

float particle::rad() const
{
	return _radius;
}

float particle::mass() const
{
	return _mass;
}

float particle::charge() const
{
	return _charge;
}

//general utility
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

//state manipulation
void particle::advance(float scalar)
{
	_velocity += _acceleration;
	_center += _velocity * scalar;
	_acceleration = vec2f(0.f, 0.f);
}

void particle::apply_newton(const std::vector<particle*> particles, float scalar)
{
	vec2f newton = vec2f(0.f, 0.f);
	for (auto p : particles)
	{
		if (p == this)
			continue;
		auto d = dist(*this, *p);
		auto r = path(*this, *p);
		auto m = p->_mass;
		newton += norm(r) * m / (d * d);
	}
	_acceleration += newton * scalar;
}

void particle::apply_coulomb(const std::vector<particle*> particles, float scalar)
{
	vec2f coulomb = vec2f(0.f, 0.f);
	for (auto p : particles)
	{
		if (p == this)
			continue;
		auto d = dist(*this, *p);
		auto r = path(*this, *p);
		auto q1 = _charge;
		auto q2 = p->_charge;
		coulomb += norm(r) * q1 * q2 / (d * d);
	}
	_acceleration += -coulomb / _mass * scalar;
}

void particle::apply_drag(float drag)
{
	_acceleration += -_velocity * drag;
}

void particle::apply_force(const vec2f& force, const vec2f& point)
{
	auto d = len(_center - point);
	if (d < 1.f)
		d = 1.f;
	_acceleration += force / _mass / d;
}
