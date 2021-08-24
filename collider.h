#pragma once

#include <vector>
#include <algorithm>
#include <random>
#include "emitter.h"

class collider
{
	const int CYCLE_COUNT = 4;

	enum class wall { none = 0, top = 1, bottom = 2, left = 3, right = 4 };

	using collision = std::pair<particle*, particle*>;
	using wall_collision = std::pair<particle*, wall>;

	std::vector<particle*> _particles;

	uint32_t _width;
	uint32_t _height;
	int _gravity = 1;
	bool _electricity = true;

	float _max_speed = 0.f;
	float _max_pos_charge = 0.f;
	float _max_neg_charge = 0.f;

	vec2f wall_project(particle* particle, wall wall);
	float wall_distance(particle* particle, wall wall);
	vec2f wall_path(particle* particle, wall wall);
	bool wall_collides(particle* particle, wall wall);
	void wall_resolve_static(particle* particle, wall wall);
	void wall_resolve_dynamic(particle* particle, wall wall);

	vec2f eval_gravity(particle* particle);
	vec2f eval_electricity(particle* particle);

	void advance_particles();
	void process_wall_collisions();
	void process_particle_collisions();
	void accelerate_particles();

	void update_stats();

	std::vector<particle*>::iterator find(const vec2f& coords);

public:
	collider(uint32_t width, uint32_t height);
	~collider();

	void operate();

	uint32_t width() const;
	uint32_t height() const;
	uint32_t count() const;
	int gravity() const;
	bool electricity() const;

	std::vector<particle*> particles() const;

	float get_mech_param(particle* particle) const;
	float get_elec_param(particle* particle) const;

	void launch(particle* particle);
	void erase(const vec2f& coords);
	particle* get(const vec2f& coords);

	void toggle_gravity();
	void toggle_electricity();
};
