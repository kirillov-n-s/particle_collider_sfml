#include "particle.h"

particle::particle(const vec2f& center, const vec2f& velocity, float radius, float mass, float charge)
	: _center(center),
	_velocity(velocity),
	_radius(radius),
	_mass(mass),
	_charge(charge)
{}

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
	_center += _velocity * scalar;
}

void particle::accelerate(const vec2f& acceleration, float scalar)
{
	_velocity += acceleration * scalar;
}
