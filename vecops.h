#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <cmath>

//generic sign function
template <typename T>
inline int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

//2d vector operators
using vec2f = sf::Vector2f;

const vec2f ZERO2 = vec2f(0.f, 0.f);
const vec2f UP = vec2f(0.f, -1.f);
const vec2f DOWN = vec2f(0.f, 1.f);
const vec2f LEFT = vec2f(-1.f, 0.f);
const vec2f RIGHT = vec2f(1.f, 0.f);

inline vec2f operator*(const vec2f& a, const vec2f& b)
{
	return vec2f(a.x * b.x, a.y * b.y);
}

inline vec2f& operator*=(vec2f& a, const vec2f& b)
{
	return a = a * b;
}

inline float dot(const vec2f& a, const vec2f& b)
{
	return a.x * b.x + a.y * b.y;
}

inline float len_sqr(const vec2f& a)
{
	return dot(a, a);
}

inline float len(const vec2f& a)
{
	return sqrt(len_sqr(a));
}

inline float dist(const vec2f& a, const vec2f& b)
{
	return len(b - a);
}

inline vec2f norm(const vec2f& a)
{
	return a / len(a);
}

inline vec2f proj(const vec2f& a, const vec2f& b)
{
	return dot(a, b) / len_sqr(b) * b;
}

inline vec2f orth(const vec2f& a)
{
	return vec2f(a.y, -a.x);
}

inline vec2f reflect(const vec2f& a, const vec2f& b)
{
	auto o = norm(orth(a));
	return a - 2.f * dot(a, o) * o;
}

//3d vector operators
using vec3f = sf::Vector3f;

const vec3f ZERO3 = vec3f(0.f, 0.f, 0.f);
const vec3f FORWARD = vec3f(0.f, 0.f, 1.f);
const vec3f BACKWARD = vec3f(0.f, 0.f, -1.f);

inline vec3f operator&(const vec3f& a, const vec3f& b)
{
	return vec3f(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x);
}

inline vec3f& operator&=(vec3f& a, const vec3f& b)
{
	return a = a & b;
}

//3d & 2d conversion
inline vec2f _2f(const vec3f& a)
{
	return vec2f(a.x, a.y);
}

inline vec3f _3f(const vec2f& a)
{
	return vec3f(a.x, a.y, 0.f);
}
