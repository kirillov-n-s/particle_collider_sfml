#include "collider.h"

//wall-related
vec2 collider::wall_project(particle* particle, wall wall)
{
	switch (wall)
	{
	case wall::top:
		return vec2(particle->_center.x, 0);
	case wall::bottom:
		return vec2(particle->_center.x, _height);
	case wall::left:
		return vec2(0, particle->_center.y);
	case collider::wall::right:
		return vec2(_width, particle->_center.y);
	}
}

float collider::wall_distance(particle* particle, wall wall)
{
	return len(wall_path(particle, wall));
}

vec2 collider::wall_path(particle* particle, wall wall)
{
	return path(particle->_center, wall_project(particle, wall));
}

bool collider::wall_collides(particle* particle, wall wall)
{
	return particle->collides(wall_project(particle, wall));
}

void collider::wall_resolve_static(particle* particle, wall wall)
{
	vec2 n = normalize(wall_path(particle, wall));
	float d = wall_distance(particle, wall) - particle->_radius;
	particle->move(n * d);
}

void collider::wall_resolve_dynamic(particle* particle, wall wall)
{
	if (wall == wall::left || wall == wall::right)
		particle->bounce(vec2(-1.f, 1.f));
	else
		particle->bounce(vec2(1.f, -1.f));
}

//combined outer acceleration applied to specific particle
vec2 collider::eval_gravity(particle* particle)
{
	if (!_gravity)
		return vec2(0.f, 0.f);
	vec2 result = vec2(0.f, 0.f);
	for (auto attractor : _particles)
	{
		if (attractor == particle)
			continue;
		auto d = particle->distance(*attractor);
		auto r = particle->path(*attractor);
		auto m = attractor->_mass;
		result += normalize(r) * m / (d * d);
	}
	return result * (float)_gravity;
}

vec2 collider::eval_electricity(particle* particle)
{
	if (!_electricity)
		return vec2(0.f, 0.f);
	vec2 result = vec2(0.f, 0.f);
	for (auto interactor : _particles)
	{
		if (interactor == particle)
			continue;
		auto d = particle->distance(*interactor);
		auto r = particle->path(*interactor);
		auto q1 = particle->_charge;
		auto q2 = interactor->_charge;
		result += normalize(r) * q1 * q2 / (d * d);
	}
	return -result / particle->_mass;
}

//stages of operating
void collider::advance_particles()
{
	for (auto p : _particles)
		p->advance();
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
		[](const auto& lhs, const auto& rhs) { return lhs->_center.x < rhs->_center.x; });

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
		float charge = p->_charge;
		if (charge > _max_pos_charge)
			_max_pos_charge = charge;
		else if (charge < _max_neg_charge)
			_max_neg_charge = charge;

		float speed = len(p->_velocity);
		if (speed > _max_speed)
			_max_speed = speed;
	}
}

//get particle by coords
std::vector<particle*>::iterator collider::get(const vec2& coords)
{
	return std::find_if(_particles.begin(), _particles.end(), [coords](const auto& p) { return p->collides(coords); });
}

//basic public interface
collider::collider(uint32_t width, uint32_t height)
	: _width(width), _height(height), _emitter(width, height) {}

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

	/*if (_counter == 0)
		accelerate_particles();
	_counter++;
	_counter %= PERIOD;*/

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

bool collider::random() const
{
	return _random;
}

float collider::sample_mass() const
{
	return _sample_mass;
}

float collider::sample_charge() const
{
	return _sample_charge;
}

std::vector<particle*> collider::particles() const
{
	return _particles;
}

//particle data
vec2 collider::get_position(particle* particle) const
{
	return particle->_center;
}

float collider::get_radius(particle* particle) const
{
	return particle->_radius;
}

float collider::get_mech_param(particle* particle) const
{
	return len(particle->_velocity) / _max_speed;
}

float collider::get_elec_param(particle* particle) const
{
	float q = particle->_charge;
	return q / (q < 0.f ? -_max_neg_charge : _max_pos_charge);
}

//particle add/remove functionality
void collider::launch()
{
	auto particle = _emitter();
	_particles.push_back(particle);
	update_stats();
}

void collider::launch(const vec2& coords)
{
	if (get(coords) != _particles.end())
		return;
	if (_random)
		_particles.push_back(_emitter(coords));
	else
		_particles.push_back(_emitter(coords, _sample_mass, _sample_charge));
	update_stats();
}

void collider::erase(const vec2& coords)
{
	auto it = get(coords);
	if (it == _particles.end())
		return;
	_particles.erase(it);
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
	_electricity = !_electricity;
}

void collider::toggle_random()
{
	_random = !_random;
}

void collider::adjust_mass(float value)
{
	_sample_mass += value;
	if (_sample_mass > MAX_RADIUS * MASS_SCALAR)
		_sample_mass = MAX_RADIUS * MASS_SCALAR;
	if (_sample_mass < MIN_RADIUS * MASS_SCALAR)
		_sample_mass = MIN_RADIUS * MASS_SCALAR;
}

void collider::adjust_charge(float value)
{
	_sample_charge += value;
}

void collider::flip_mass()
{
	_sample_mass = _sample_mass > MID_RADIUS * MASS_SCALAR ? MIN_RADIUS * MASS_SCALAR : MAX_RADIUS * MASS_SCALAR;
}

void collider::flip_charge()
{
	_sample_charge = -_sample_charge;
}

void collider::average_mass()
{
	_sample_mass = MID_RADIUS * MASS_SCALAR;
}

void collider::nullify_charge()
{
	_sample_charge = 0.f;
}

particle* collider::construct_sample(const vec2& coords)
{
	float mass = _sample_mass;
	return new particle(coords, vec2(MAX_VELOCITY, MAX_VELOCITY), mass / MASS_SCALAR, mass, _sample_charge);
}
