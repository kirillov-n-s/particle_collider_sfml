#include "collider.h"

bool collider::in_bounds(particle* particle) const
{
	auto pos = particle->pos();
	return pos.x > 0.f && pos.x < _width && pos.y > 0.f && pos.y < _height;
}

//wall-related
vec2f collider::wall_project(particle* particle, wall wall)
{
	switch (wall)
	{
	case wall::top:
		return vec2f(particle->pos().x, 0);
	case wall::bottom:
		return vec2f(particle->pos().x, _height);
	case wall::left:
		return vec2f(0, particle->pos().y);
	case collider::wall::right:
		return vec2f(_width, particle->pos().y);
	}
}

float collider::wall_distance(particle* particle, wall wall)
{
	return len(wall_path(particle, wall));
}

vec2f collider::wall_path(particle* particle, wall wall)
{
	return wall_project(particle, wall) - particle->pos();
}

bool collider::wall_collides(particle* particle, wall wall)
{
	auto pos = particle->pos();
	auto rad = particle->rad();
	switch (wall)
	{
	case wall::top:
		return pos.y < rad;
	case wall::bottom:
		return pos.y > _height - rad;
	case wall::left:
		return pos.x < rad;
	case wall::right:
		return pos.x > _width - rad;
	}
}

void collider::wall_resolve_static(particle* particle, wall wall)
{
	vec2f n = norm(wall_path(particle, wall));
	float d = wall_distance(particle, wall);
	float r = particle->rad();
	d += !in_bounds(particle) ? r : -r;
	particle->move(n * d);
}

void collider::wall_resolve_dynamic(particle* particle, wall wall)
{
	if (wall == wall::left || wall == wall::right)
		particle->bounce(vec2f(-1.f, 1.f));
	else
		particle->bounce(vec2f(1.f, -1.f));
}

//acceleration by inner forces
vec2f collider::eval_gravity(particle* particle)
{
	if (!_gravity)
		return vec2f(0.f, 0.f);
	vec2f result = vec2f(0.f, 0.f);
	for (auto attractor : _particles)
	{
		if (attractor == particle)
			continue;
		auto d = dist(*particle, *attractor);
		auto r = path(*particle, *attractor);
		auto m = attractor->mass();
		result += norm(r) * m / (d * d);
	}
	return result * (float)_gravity;
}

vec2f collider::eval_electricity(particle* particle)
{
	if (!_electricity)
		return vec2f(0.f, 0.f);
	vec2f result = vec2f(0.f, 0.f);
	for (auto interactor : _particles)
	{
		if (interactor == particle)
			continue;
		auto d = dist(*particle, *interactor);
		auto r = path(*particle, *interactor);
		auto q1 = particle->charge();
		auto q2 = interactor->charge();
		result += norm(r) * q1 * q2 / (d * d);
	}
	return -result / particle->mass();
}

vec2f collider::eval_drag(particle* particle)
{
	if (!_drag)
		return vec2f(0.f, 0.f);
	return -particle->vel() * particle->rad() / MID_RADIUS;
}

//stages of operating
void collider::advance_particles()
{
	for (auto p : _particles)
		p->advance(1.f / CYCLE_COUNT);
}

void collider::process_wall_collisions()
{
	std::vector<std::pair<particle*, wall>> wall_colliding;

	for (auto& p : _particles)
		for (int w = 1; w <= (int)wall::right; w++)
			if (wall_collides(p, (wall)w))
				wall_colliding.push_back(std::make_pair(p, (wall)w));

	for (auto& p : wall_colliding)
		wall_resolve_static(p.first, p.second);
	for (auto& p : wall_colliding)
		wall_resolve_dynamic(p.first, p.second);
}

void collider::process_particle_collisions()
{
	if (_particles.size() < 2)
		return;

	std::vector<particle*> active;
	std::vector<collision> colliding;

	std::sort(_particles.begin(), _particles.end(),
		[](const auto& lhs, const auto& rhs)
		{
			return lhs->pos().x < rhs->pos().x;
		});

	active.push_back(_particles.front());
	for (auto it = _particles.begin() + 1; it != _particles.end(); it++)
	{
		bool res = false;

		for (const auto& p : active)
		{
			bool inter = intersects_x(*p, **it);
			res |= inter;
			if (inter && collides(*p, **it))
				colliding.push_back(std::make_pair(p, *it));
		}

		if (!res)
			active.clear();
		active.push_back(*it);
	}

	/*int n = _particles.size();
	for (int i = 0; i < n - 1; i++)
	{
		for (int j = i + 1; j < n; j++)
		{
			auto one = _particles[i];
			auto other = _particles[j];
			if (one->collides(*other))
				colliding.push_back(std::make_pair(one, other));
		}
	}*/

	for (auto& pair : colliding)
		resolve_static(*pair.first, *pair.second);
	for (auto& pair : colliding)
		resolve_dynamic(*pair.first, *pair.second);
}

void collider::accelerate_particles()
{
	for (auto p : _particles)
	{
		auto acceleration = eval_gravity(p) * GRAVITY_SCALAR + eval_electricity(p) * ELECTRICITY_SCALAR;
		auto drag = eval_drag(p) * DRAG_SCALAR;
		p->accelerate(len_sqr(acceleration) > len_sqr(drag) ? acceleration : drag);
	}
}

void collider::purge_particles()
{
	auto it = std::remove_if(_particles.begin(), _particles.end(),
		[this](const auto& val)
		{
			return !in_bounds(val);
		});
	if (it != _particles.end())
		_particles.erase(it, _particles.end());
}

//stats update
void collider::update_stats()
{
	for (auto& p : _particles)
	{
		float charge = p->charge();
		if (charge > _max_pos_charge)
			_max_pos_charge = charge;
		else if (charge < _max_neg_charge)
			_max_neg_charge = charge;

		float speed = len(p->vel());
		if (speed > _max_speed)
			_max_speed = speed;
	}
}

//public interface
collider::collider(uint32_t width, uint32_t height)
	: _width(width), _height(height) {}

collider::~collider()
{
	clear();
}

void collider::operate()
{
	for (int i = 0; i < CYCLE_COUNT; i++)
	{
		advance_particles();
		process_wall_collisions();
		process_particle_collisions();
	}

	accelerate_particles();
	purge_particles();

	update_stats();
}

//properties
uint32_t collider::width() const
{
	return _width;
}

uint32_t collider::height() const
{
	return _height;
}

uint32_t collider::count() const
{
	return _particles.size();
}

//inner forces
int collider::get_gravity() const
{
	return _gravity;
}

bool collider::get_electricity() const
{
	return _electricity;
}

bool collider::get_drag() const
{
	return _drag;
}

void collider::toggle_gravity()
{
	if (_gravity == 1)
		_gravity = -1;
	else
		_gravity++;
}

void collider::toggle_electricity()
{
	_electricity ^= true;
}

void collider::toggle_drag()
{
	_drag ^= true;
}

//particle access & manip
std::vector<particle*> collider::particles() const
{
	return _particles;
}

void collider::launch(particle* particle)
{
	_particles.push_back(particle);
	update_stats();
}

void collider::erase(particle* particle)
{
	auto it = std::find(_particles.begin(), _particles.end(), particle);
	if (it == _particles.end())
		return;
	_particles.erase(it);
	_max_speed = _max_pos_charge = _max_neg_charge = 0.f;
	update_stats();
}

void collider::erase()
{
	_particles.pop_back();
	_max_speed = _max_pos_charge = _max_neg_charge = 0.f;
	update_stats();
}

particle* collider::get(const vec2f& coords) const
{
	auto it = std::find_if(_particles.begin(), _particles.end(),
		[coords](const auto& p)
		{
			return p->contains(coords);
		});
	if (it == _particles.end())
		return nullptr;
	return *it;
}

void collider::clear()
{
	for (auto& p : _particles)
		delete p;
	_particles.clear();
	_max_speed = _max_pos_charge = _max_neg_charge = 0.f;
}

//particle stats
float collider::get_mech_stat(particle* particle) const
{
	return len(particle->vel()) / _max_speed;
}

float collider::get_elec_stat(particle* particle) const
{
	float q = particle->charge();
	return q / (q < 0.f ? -_max_neg_charge : _max_pos_charge);
}

//instant force
void collider::apply_force(const vec2f& point, const vec2f& vec)
{
	auto force = vec * FORCE_SCALAR;
	for (auto& p : _particles)
	{
		auto m = p->mass();
		auto r = len(p->pos() - point);
		if (r < 1.f)
			r = 1.f;
		p->accelerate(force / m / r);
	}
}
