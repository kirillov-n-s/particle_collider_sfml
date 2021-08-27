#pragma once
#include <vector>
#include <algorithm>
#include "emitter.h"

class collider
{
	const int CYCLE_COUNT = 4;

	enum class wall
	{
		none,
		top,
		bottom,
		left,
		right
	};

	using collision = std::pair<particle*, particle*>;
	using wall_collision = std::pair<particle*, wall>;

	std::vector<particle*> _particles;

	uint32_t _width;
	uint32_t _height;

	int _gravity = 1;
	bool _electricity = true;
	bool _drag = true;

	vec2f _g_field = vec2f(0.f, 0.f);
	vec3f _em_field = vec3f(0.f, 0.f, 0.f);

	float _max_speed = 0.f;
	float _max_pos_charge = 0.f;
	float _max_neg_charge = 0.f;

	bool in_bounds(particle* particle) const;

	vec2f wall_project(particle* particle, wall wall);
	float wall_distance(particle* particle, wall wall);
	vec2f wall_path(particle* particle, wall wall);
	bool wall_collides(particle* particle, wall wall);
	void wall_resolve_static(particle* particle, wall wall);
	void wall_resolve_dynamic(particle* particle, wall wall);

	vec2f eval_gravity(particle* particle);
	vec2f eval_electricity(particle* particle);
	vec2f eval_drag(particle* particle);

	void advance_particles();
	void process_wall_collisions();
	void process_particle_collisions();
	void accelerate_particles();
	void purge_particles();

	void update_stats();

public:
	collider(uint32_t width, uint32_t height);
	~collider();

	void operate();

	uint32_t width() const;
	uint32_t height() const;
	uint32_t count() const;

	int get_gravity() const;
	bool get_electricity() const;
	bool get_drag() const;

	void toggle_gravity();
	void toggle_electricity();
	void toggle_drag();

	std::vector<particle*> particles() const;
	void launch(particle* particle);
	void erase(particle* particle);
	void erase();
	particle* get(const vec2f& coords) const;
	void clear();

	float get_mech_stat(particle* particle) const;
	float get_elec_stat(particle* particle) const;

	void apply_force(const vec2f& point, const vec2f& vec);
};
