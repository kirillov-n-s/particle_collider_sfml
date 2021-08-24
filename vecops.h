#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>

//generic sign function
template <typename T>
inline int sgn(T val)
{
	return (T(0) < val) - (val < T(0));
}

//2d vector operators
using vec2f = sf::Vector2f;

inline vec2f operator*(const vec2f& lhs, const vec2f& rhs)
{
	return vec2f(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline vec2f& operator*=(vec2f& lhs, const vec2f& rhs)
{
	return lhs = lhs * rhs;
}

inline float dot(const vec2f& a, const vec2f& b)
{
	return a.x * b.x + a.y * b.y;
}

inline float len_sqr(const vec2f& vec)
{
	return dot(vec, vec);
}

inline float len(const vec2f& vec)
{
	return sqrt(len_sqr(vec));
}

inline float dist(const vec2f& a, const vec2f& b)
{
	return len(b - a);
}

inline vec2f norm(const vec2f& vec)
{
	return vec / len(vec);
}

inline vec2f sgn(const vec2f& vec)
{
	return vec2f(sgn(vec.x), sgn(vec.y));
}

inline vec2f abs(const vec2f& vec)
{
	return vec2f(abs(vec.x), abs(vec.y));
}