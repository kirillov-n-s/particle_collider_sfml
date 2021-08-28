#include "particle.h"

//particle pair inter-data
float dist(const particle& p1, const particle& p2)
{
	return ::dist(p1._center, p2._center);
}

vec2f path(const particle& from, const particle& to)
{
	return to._center - from._center;
}

bool collides(const particle& p1, const particle& p2)
{
	return dist(p1, p2) < p1._radius + p2._radius;
}

bool intersects_x(const particle& p1, const particle& p2)
{
	auto a1 = p1._center.x - p1._radius;
	auto b1 = p1._center.x + p1._radius;

	auto a2 = p2._center.x - p2._radius;
	auto b2 = p2._center.x + p2._radius;

	return a2 >= a1 && a2 <= b1 || b2 >= a1 && b2 <= b1
		|| a1 >= a2 && a1 <= b2 || b1 >= a2 && b1 <= b2;
}

bool intersects_y(const particle& p1, const particle& p2)
{
	auto a1 = p1._center.y - p1._radius;
	auto b1 = p1._center.y + p1._radius;

	auto a2 = p2._center.y - p2._radius;
	auto b2 = p2._center.y + p2._radius;

	return a2 >= a1 && a2 <= b1 || b2 >= a1 && b2 <= b1
		|| a1 >= a2 && a1 <= b2 || b1 >= a2 && b1 <= b2;
}

//collision resolution
void resolve_static(particle& p1, particle& p2)
{
	vec2f n = norm(path(p1, p2));
	float d = 0.5f * (dist(p1, p2) - p1._radius - p2._radius);
	vec2f v = n * d;
	p1.move(v);
	p2.move(-v);
	p1._acceleration += proj(p2._acceleration, v);
	p2._acceleration += proj(p1._acceleration, v);
}

void resolve_dynamic(particle& p1, particle& p2)
{
	auto m1 = p1._mass;
	auto m2 = p2._mass;
	auto msum = m1 + m2;

	auto v1 = p1._velocity;
	auto v2 = p2._velocity;

	auto q1 = p1._charge;
	auto q2 = p2._charge;

	p1._velocity = (m1 - m2) / msum * v1 + 2.f * m2 / msum * v2;
	p2._velocity = 2.f * m1 / msum * v1 + (m2 - m1) / msum * v2;
	p1._charge = p2._charge = (q1 + q2) / 2.f;
}
