#pragma once

#include <vector>
#include <algorithm>
#include <random>
#include "emitter.h"

class collider
{
	enum class wall { none = 0, top = 1, bottom = 2, left = 3, right = 4 };

	using collision = std::pair<particle*, particle*>;
	using wall_collision = std::pair<particle*, wall>;

	std::vector<particle*> _particles;
	emitter _emitter;

	uint32_t _width;
	uint32_t _height;
	int _gravity = 1;
	bool _electricity = true;

	float _max_speed = 0.f;
	float _max_pos_charge = 0.f;
	float _max_neg_charge = 0.f;

	bool _random = true;
	float _sample_mass = MIN_RADIUS * MASS_SCALAR;
	float _sample_charge = 0.f;

	int _counter = 0;

	vec2 wall_project(particle* particle, wall wall);
	float wall_distance(particle* particle, wall wall);
	vec2 wall_path(particle* particle, wall wall);
	bool wall_collides(particle* particle, wall wall);
	void wall_resolve_static(particle* particle, wall wall);
	void wall_resolve_dynamic(particle* particle, wall wall);

	vec2 eval_gravity(particle* particle);
	vec2 eval_electricity(particle* particle);

	void advance_particles();
	void process_wall_collisions();
	void process_particle_collisions();
	void accelerate_particles();

	void update_stats();

	std::vector<particle*>::iterator get(const vec2& coords);

public:
	collider(uint32_t width, uint32_t height);
	~collider();

	void operate();

	uint32_t width() const;
	uint32_t height() const;
	uint32_t count() const;
	int gravity() const;
	bool electricity() const;

	bool random() const;
	float sample_mass() const;
	float sample_charge() const;

	std::vector<particle*> particles() const;

	vec2 get_position(particle* particle) const;
	float get_radius(particle* particle) const;
	float get_mech_param(particle* particle) const;
	float get_elec_param(particle* particle) const;

	void launch();
	void launch(const vec2& coords);
	void erase(const vec2& coords);

	void toggle_gravity();
	void toggle_electricity();

	void toggle_random();
	void adjust_mass(float value);
	void adjust_charge(float value);
	void flip_mass();
	void flip_charge();
	void average_mass();
	void nullify_charge();
	particle* construct_sample(const vec2& coords);
};
