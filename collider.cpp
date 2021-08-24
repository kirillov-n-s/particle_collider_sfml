#include "collider.h"

//wall-related
vec2f collider::wall_project(particle* particle, wall wall)
{
	switch (wall)
	{
	case wall::top:
		return vec2f(particle->get_pos().x, 0);
	case wall::bottom:
		return vec2f(particle->get_pos().x, _height);
	case wall::left:
		return vec2f(0, particle->get_pos().y);
	case collider::wall::right:
		return vec2f(_width, particle->get_pos().y);
	}
}

float collider::wall_distance(particle* particle, wall wall)
{
	return len(wall_path(particle, wall));
}

vec2f collider::wall_path(particle* particle, wall wall)
{
	return wall_project(particle, wall) - particle->get_pos();
}

bool collider::wall_collides(particle* particle, wall wall)
{
	return particle->contains(wall_project(particle, wall));
}

void collider::wall_resolve_static(particle* particle, wall wall)
{
	vec2f n = norm(wall_path(particle, wall));
	float d = wall_distance(particle, wall) - particle->get_rad();
	particle->move(n * d);
}

void collider::wall_resolve_dynamic(particle* particle, wall wall)
{
	if (wall == wall::left || wall == wall::right)
		particle->bounce(vec2f(-1.f, 1.f));
	else
		particle->bounce(vec2f(1.f, -1.f));
}

//combined acceleration by various forces
vec2f collider::eval_gravity(particle* particle)
{
	if (!_gravity)
		return vec2f(0.f, 0.f);
	vec2f result = vec2f(0.f, 0.f);
	for (auto attractor : _particles)
	{
		if (attractor == particle)
			continue;
		auto d = particle->dist(*attractor);
		auto r = particle->path(*attractor);
		auto m = attractor->get_mass();
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
		auto d = particle->dist(*interactor);
		auto r = particle->path(*interactor);
		auto q1 = particle->get_charge();
		auto q2 = interactor->get_charge();
		result += norm(r) * q1 * q2 / (d * d);
	}
	return -result / particle->get_mass();
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
		[](const auto& lhs, const auto& rhs) { return lhs->get_pos().x < rhs->get_pos().x; });

	active.push_back(_particles.front());
	for (auto it = _particles.begin() + 1; it != _particles.end(); it++)
	{
		bool res = false;

		for (const auto& p : active)
		{
			bool inter = p->intersects_x(**it);
			res |= inter;
			if (inter && p->collides(**it))
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

	for (auto& p : colliding)
		p.first->resolve_static(*p.second);
	for (auto& p : colliding)
		p.first->resolve_dynamic(*p.second);
}

void collider::accelerate_particles()
{
	for (auto p : _particles)
		p->accelerate(eval_gravity(p) * GRAVITY_SCALAR + eval_electricity(p) * ELECTRICITY_SCALAR);
}

//stats update
void collider::update_stats()
{
	for (auto& p : _particles)
	{
		float charge = p->get_charge();
		if (charge > _max_pos_charge)
			_max_pos_charge = charge;
		else if (charge < _max_neg_charge)
			_max_neg_charge = charge;

		float speed = len(p->get_vel());
		if (speed > _max_speed)
			_max_speed = speed;
	}
}

//get particle by coords
std::vector<particle*>::iterator collider::find(const vec2f& coords)
{
	return std::find_if(_particles.begin(), _particles.end(), [coords](const auto& p) { return p->contains(coords); });
}

//public interface
collider::collider(uint32_t width, uint32_t height)
	: _width(width), _height(height) {}

collider::~collider()
{
	for (auto& p : _particles)
		delete p;
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

int collider::gravity() const
{
	return _gravity;
}

bool collider::electricity() const
{
	return _electricity;
}

std::vector<particle*> collider::particles() const
{
	return _particles;
}

//particle data
float collider::get_mech_param(particle* particle) const
{
	return len(particle->get_vel()) / _max_speed;
}

float collider::get_elec_param(particle* particle) const
{
	float q = particle->get_charge();
	return q / (q < 0.f ? -_max_neg_charge : _max_pos_charge);
}

//particle access
void collider::launch(particle* particle)
{
	_particles.push_back(particle);
	update_stats();
}

void collider::erase(const vec2f& coords)
{
	auto it = find(coords);
	if (it == _particles.end())
		return;
	_particles.erase(it);
}

particle* collider::get(const vec2f& coords)
{
	auto it = find(coords);
	if (it == _particles.end())
		return nullptr;
	return *it;
}

//forces toggle
void collider::toggle_gravity()
{
	if (_gravity == 1)
		_gravity = -1;
	else
		_gravity++;
}

void collider::toggle_electricity()
{
	_electricity = !_electricity;
}
