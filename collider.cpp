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
	case wall::right:
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
//vec2f collider::eval_gravity(particle* particle)
//{
//	if (!_newton)
//		return ZERO2;
//	vec2f result = ZERO2;
//	for (auto attractor : _particles)
//	{
//		if (attractor == particle)
//			continue;
//		auto d = dist(*particle, *attractor);
//		auto r = path(*particle, *attractor);
//		auto m = attractor->mass();
//		result += norm(r) * m / (d * d);
//	}
//	return result * (float)_newton;
//}
//
//vec2f collider::eval_electricity(particle* particle)
//{
//	if (!_coulomb)
//		return ZERO2;
//	vec2f result = ZERO2;
//	for (auto interactor : _particles)
//	{
//		if (interactor == particle)
//			continue;
//		auto d = dist(*particle, *interactor);
//		auto r = path(*particle, *interactor);
//		auto q1 = particle->charge();
//		auto q2 = interactor->charge();
//		result += norm(r) * q1 * q2 / (d * d);
//	}
//	return -result / particle->mass();
//}
//
//vec2f collider::eval_drag(particle* particle)
//{
//	if (!_drag)
//		return ZERO2;
//	return -particle->velocity();
//}

//stages of operating
void collider::advance_particles()
{
	for (auto p : _particles)
		p->advance(1.f / CYCLES);
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
		if (_newton)
			p->apply_newton(_particles, NEWTON * _newton);
		if (_coulomb)
			p->apply_coulomb(_particles, COULOMB);
		if (_drag)
			p->apply_drag(1.f / 60.f);
		p->apply_gravity(_Gfield * GRAVITY);
		p->apply_lorentz(_Efield * ELECTRO, _Bfield * MAGNET);
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
	_max_speed = _max_pos_charge = _max_neg_charge = 0.f;
	for (auto p : _particles)
	{
		float charge = p->charge();
		if (charge > _max_pos_charge)
			_max_pos_charge = charge;
		else if (charge < _max_neg_charge)
			_max_neg_charge = charge;

		float speed = len(p->velocity());
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
	for (int i = 0; i < CYCLES; i++)
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
int collider::get_newton() const
{
	return _newton;
}

bool collider::get_coulomb() const
{
	return _coulomb;
}

bool collider::get_drag() const
{
	return _drag;
}

void collider::toggle_newton()
{
	if (_newton == 1)
		_newton = -1;
	else
		_newton++;
	if (_newton == 1)
		_drag = false;
}

void collider::toggle_coulomb()
{
	_coulomb ^= true;
}

void collider::toggle_drag()
{
	_drag ^= true;
	if (_drag && _newton == 1)
		_newton = 0;
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
	//_max_speed = _max_pos_charge = _max_neg_charge = 0.f;
	update_stats();
}

void collider::erase()
{
	if (_particles.empty())
		return;
	_particles.pop_back();
	//_max_speed = _max_pos_charge = _max_neg_charge = 0.f;
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
float collider::mech_stat(particle* particle) const
{
	return len(particle->velocity()) / _max_speed;
}

float collider::elec_stat(particle* particle) const
{
	float q = particle->charge();
	return q / (q < 0.f ? -_max_neg_charge : _max_pos_charge);
}

//instant force
void collider::apply_force(const vec2f& point, const vec2f& vec)
{
	auto force = vec * FORCE;
	for (auto& p : _particles)
		p->apply_force(force, point);
}

//constant forces
vec2f collider::get_G_field() const
{
	return _Gfield;
}

vec3f collider::get_EM_field() const
{
	return _3f(_Efield) + _Bfield;
}

void collider::set_G_field(const vec2f& dir)
{
	_Gfield = dir;
}

void collider::set_E_field(const vec2f& dir)
{
	_Efield = dir;
}

void collider::set_B_field(const vec3f& dir)
{
	_Bfield = dir;
}
